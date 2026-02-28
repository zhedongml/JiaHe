#include <opencv2\imgproc\types_c.h>
#include"SlantedCross.h"

SLANDEDCROSS_BEGIN

void SetCrossPixel(double pxl)
{
	SlantedCross::pixelc = pxl;
	//qDebug() << "zzz..." << SlantedCross::pixelc;
	//memcpy(&SlantedCross::pixelc, &pxl, sizeof(double));
}
double getCrossPixel()
{
    return SlantedCross::pixelc * SlantedCross::binning;
}

void SetBinning(int binning)
{
    SlantedCross::binning = binning;
}

int getBinning()
{
    return SlantedCross::binning;
}

//检查ROI区域方向以及对比度
int calculate_mtf_slantedcross(Mat& roi, double** Freq, double** Mtf, double** Esf_Lsf_x, double** Esf, double** Lsf, int* bin_len, double* esf_len, double* slope)
{
	double *farea = NULL;//原始数据数组

	unsigned char direction;//斜边方向
	int size_x = 0, size_y = 0;//ROI区域大小
	int len, err;
	int center;
	int numcycles = 0;
	double off, R2;

	//cout << roi.channels() << endl;
	if (check_roi(roi, direction))
	{
		size_x = roi.cols;
		size_y = roi.rows;

		len = size_x*size_y;
		farea = (double *)malloc(len * sizeof(double));
		roi_normilized(roi, farea, size_x, size_y, len);

		///* calculate the sfr on this area */
		err = mtfProc(Freq, Mtf, Esf_Lsf_x, Esf, Lsf, bin_len, esf_len, farea, (unsigned short)size_x, &size_y, slope, &numcycles, &center, &off, &R2);
		if ((fabs(atan(*slope)) * 180 / M_PI) < 1)
			*esf_len = quick_esf(roi);
	} 
	else
	{
		direction = -1;
	}
	 
	free(farea);
	//free(Freq);
	//free(Mtf);
	//free(Esf);
	//free(Lsf);

	return(direction);

}  



double quick_esf(Mat img)
{
	uchar direction;
	Mat roi = img.clone();
	Mat mean;
	Mat cov;
	double *esf, *esf_x;

	check_roi(roi, direction);
	cv::calcCovarMatrix(roi, cov, mean, CV_COVAR_NORMAL | CV_COVAR_ROWS);

	int size_x = mean.cols;
	int size_y = mean.rows;
	int len = size_x*size_y;

	esf = (double *)malloc(len * sizeof(double));
	esf_x = (double *)malloc(len * sizeof(double));
	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;

	minMaxIdx(mean, minp, maxp);

	int num = 0;
	for (int i = 0; i < size_y; i++)
	{
		for (int j = 0; j < size_x; j++)
		{
			esf[num] = (mean.at<double>(i, j) - minv) / (maxv - minv);
			esf_x[num] = num;
			//cout << esf[num] << endl;
			num++;
		}

	}



	if (esf[0] > esf[len - 1]) {
		reverse(esf, esf + len);
	}

	double esf_len = 0;// calculate_esf(esf, esf_x, len, 0.1, 0.9);//20200717

	//free(img_data_free);
	free(esf);
	free(esf_x);


	return esf_len;


}

bool check_roi(Mat& roi, unsigned char& direction)
{
	if (roi.channels() == 3)
		cvtColor(roi, roi, CV_RGB2GRAY);
	Mat img_threshold, dst8bit;

	dst8bit = roi.clone();

	if (dst8bit.depth() == CV_16U)
	{
		Mat outtmp;// = Mat(roi.cols, roi.rows, CV_8UC1);

		double minv = 0.0, maxv = 0.0;
		double* minp = &minv;
		double* maxp = &maxv;
		minMaxIdx(dst8bit, minp, maxp);

		if (maxv>4095.1)
		{
			cv::Mat tmp = dst8bit / 65535 * 255;

			tmp.convertTo(outtmp, CV_8UC1);
		}
		else
		{
			cv::Mat tmp = dst8bit / 4095.0 * 255;

			tmp.convertTo(outtmp, CV_8UC1);
		}

		dst8bit = outtmp;

	}
	//threshold(roi, img_threshold, 0, 255, CV_THRESH_OTSU);
	img_threshold = dst8bit.clone();
	Mat element = getStructuringElement(MORPH_RECT, Size(11, 11));
	//dilate(img_threshold, img_threshold, element);

	Mat row0 = img_threshold.rowRange(0, 1);
	Mat col0 = img_threshold.colRange(0, 1);

	double minvR = 0.0, maxvR = 0.0;
	double minvC = 0.0, maxvC = 0.0;
	Point minpR, maxpR, minpC, maxpC;

	minMaxLoc(row0, &minvR, &maxvR, &minpR, &maxpR);
	minMaxLoc(col0, &minvC, &maxvC, &minpC, &maxpC);


	//if (maxvR == 0 && maxvC != 0)
	if (maxvC>maxvR && (maxvC - maxvR) / (maxvC + maxvR) > 0.01)
	{
		direction = HORIZONTAL;
		transpose(roi, roi);
		return true;
	}

	//else if (maxvR != 0 && maxvC == 0)
	else if (maxvR>maxvC && (maxvR - maxvC) / (maxvC + maxvR) > 0.01)
	{
		direction = VERTICAL;
		return true;
	}

	else
		return false;

}
// 归一化ROI数据到0~1

double CalculateYdata(cv::Mat start)
{
    // cv::Mat start = row1.colRange(0, 3);
    double startVal = 0;
    double maxVal = 0, minVal = 0;
    Point maxLoc, minLoc;
    minMaxLoc(start, &minVal, &maxVal, NULL, &maxLoc);
    if (maxVal - minVal > 10)
    {
        cv::Scalar sum = cv::sum(start);
        startVal = (sum(0) - maxVal) / 2.0;
    }
    else
    {
        cv::Scalar sum = cv::sum(start);
        startVal = sum(0) / 3.0;
    }
    return startVal;
}

cv::Mat BackgroundSubstract(cv::Mat roi)
{
    if (roi.channels() == 3)
    {
        cvtColor(roi, roi, CV_RGB2GRAY);
    }
    int row = roi.rows;
    int col = roi.cols;
    cv::Mat dst = cv::Mat::zeros(roi.size(), roi.type());

    // calculate start value
    for (int ii = 0; ii < row; ii++)
    {
        cv::Mat row1 = roi.rowRange(ii, ii + 1);
        cv::Mat start = row1.colRange(0, 3);
        cv::Mat end = row1.colRange(col - 3, col);
        double startVal = CalculateYdata(start);
        double endVal = CalculateYdata(end);
        cv::Point startP = cv::Point(1, startVal);
        cv::Point endP = cv::Point(col, endVal);

        vector<cv::Point> fitdata;
        fitdata.push_back(startP);
        fitdata.push_back(endP);
        cv::Vec4f line;
        cv::fitLine(fitdata, line, cv::DIST_L1, 0, 1e-2, 1e-2);
        cv::Mat background = cv::Mat::zeros(cv::Size(col, 1), CV_32F);
        for (int i = 0; i < col; i++)
        {
            cv::Point point0;
            point0.x = line[2];
            point0.y = line[3];
            double k = line[1] / line[0];
            float tmp = k * (i - point0.x) + point0.y;
            background.at<float>(0, i) = tmp;
        }
        background.convertTo(background, row1.type());
        cv::Mat rowafter = row1 - background;
        cv::Mat dstrows = dst.rowRange(ii, ii + 1);
        rowafter.copyTo(dstrows);
        // dst.rowRange(ii, ii + 1) = rowafter.clone();
    }

    return dst;
}

bool roi_normilized(Mat &roi, double* farea, int& size_x, int& size_y, int& len)
{
	//cout << roi << endl;
	//uchar* img_data_free;
	//uchar* img_data = (uchar *)malloc(len * sizeof(uchar));
	//img_data_free = img_data;

	//if (roi.isContinuous())
	//	img_data = roi.data;

	// add the backgroud

    // roi = BackgroundSubstract(roi);

	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;

	minMaxIdx(roi, minp, maxp);


	Mat roi_int;
	roi.convertTo(roi_int, CV_32SC1);
	int r = roi_int.rows;
	int c = roi_int.cols;

	int dex = 0;
	for (int i = 0; i < roi_int.rows; i++)
	{
		for (int j = 0; j < roi_int.cols; j++)
		{
			//cout << roi_int.at<int>(i+1, j+1) << endl;
			farea[dex] = double((roi_int.at<int>(i, j) - minv)/ (maxv - minv));
			dex++;
		}
	}

	//for (int dex = 0; dex < len; dex++)
	//{
	//	farea[dex] = (img_data[dex] - minv) / (maxv - minv);
	//	//cout << (double)img_data[dex] << endl;
	//	//cout << farea[dex] << endl;
	//	//farea[dex] = img_data[dex];
	//}

	//free(img_data_free);

	return true;
}


int mtfProc(double **freq, double **mtf, double **esf_lsf_x, double **esf, double **lsf,
	int *len, double *esf_len,
	double *farea,
	unsigned short size_x, int *nrows,
	double *slope, int *numcycles, int *pcnt2, double *off, double *R2)
{
	unsigned short i, j, col, err = 0;
	long pcnt;
	double dt, dt1, sfrc, tmp, tmp2;
	double *temp = NULL, *shifts = NULL, *edgex = NULL, *Signal = NULL;
	double *AveEdge = NULL, *AveTmp = NULL;
	long *counts = NULL;
	int nzero;
	unsigned short size_y;
	int bin_len;
	double avar, bvar, offset1, offset2, offset;
	double centroid;
	int start_row, center_row;
	double *farea_old;
	double cyclelimit;
	FILE *fp = NULL;
	int half_window = 2;//平滑窗口大小
	size_y = *nrows;
	int diff_window = 3 ;//微分窗口大小
	//ROI区域行为偶数
	//if (size_x % 2 != 0) {
	//	fprintf(stderr, "ROI width is not even.  Does this really matter???\n");
	//	return 1;
	//}

	//最小所需行数
	cyclelimit = 5.0;

	shifts = (double *)malloc(size_y * sizeof(double));
	temp = (double *)malloc(size_y * sizeof(double));
	edgex = (double *)malloc(size_y*size_x * sizeof(double));
	Signal = (double *)malloc(size_y*size_x * sizeof(double));


	// 定位质心
	err = locate_centroids(farea, temp, shifts, size_x, size_y, &offset1,true);
	if (err != 0) { return 2; }

	//质心直线拟合
	err = fit(size_y, temp, shifts, slope, &offset2, R2, &avar, &bvar);
	if (err != 0) { return 3; }



	
	// 检查刀口斜率， 以确保后面超采样的质量
	err = check_slope(*slope, &size_y, numcycles, cyclelimit, 1);

	
	// 调整ROI的行
	center_row = *nrows / 2;
	start_row = center_row - size_y / 2;
	farea_old = farea;
	farea = farea + start_row*size_x;
	
	// 计算刀口的偏移
	offset = offset1  + offset2 - size_x / 2; // 为什么要加0.5？2019.03.26添加：因为dt是两个相邻点的关系， 所以dt的坐标应该加0.5

	*off = offset;

	if (err != 0) {   
		*pcnt2 = 2 * size_x; 
		return 4;
	}


	// 基于拟合结果更新shifts
	col = size_y / 2;
	for (i = 0; i < size_y; i++) {
		shifts[i] = (*slope) * (double)(i - col) + offset;
	}
		
	//归一化
	dt = 0.0;
	dt1 = 1.0;

	pcnt = 0;
	for (j = 0; j < size_y; j++) {
		for (i = 0; i < size_x; i++) {
			edgex[pcnt] = (double)i - shifts[j];// 计算每个点离刀口的距离
			Signal[pcnt] = ((farea[((j*(long)size_x) + i)]) - dt) / (dt1 - dt);// 归一化每个点的亮度
			pcnt++;
		}
	}


	bin_len = (int)(ALPHA*size_x);
	AveEdge = (double *)malloc(bin_len * sizeof(double));
	AveTmp = (double *)malloc(bin_len * sizeof(double));
	counts = (long *)malloc(bin_len * sizeof(long));


	//初始化ESF,LSF横坐标
	(*esf_lsf_x) = (double *)malloc(2 * bin_len * sizeof(double));
	for (int i = 0; i < 2 * bin_len; i++)
	{
		(*esf_lsf_x)[i] = i*pixelc / ALPHA * 1000;
	}

	// 进行超采样， 生成长度为size_x*ALPHA（4）的单行图像(ESF), 保存在AveEdge中
	nzero = bin_to_regular_xgrid((unsigned short)ALPHA, edgex, Signal,
		AveEdge, counts,
		size_x, size_y);
	free(counts);
	free(Signal);
	free(edgex);

	//翻转ESF
	//if (AveEdge[0] > AveEdge[bin_len - 1]) {
	//	reverse(AveEdge, AveEdge + bin_len);
	//}
	//gamma_correct(AveEdge, 0.5, bin_len);


	


	// 将ESF转换为差分图LSF， 并计算LSF的质心
	//calculate_derivative_old(bin_len, AveTmp, AveEdge, &centroid, 0);
	//calculate_derivative_new(bin_len, AveTmp, AveEdge, &centroid, diff_window);
	

	calculate_esf(bin_len, AveTmp, AveEdge);

	(*esf) = (double *)malloc(bin_len * sizeof(double));


	




	// 寻找LSF的最大值点
	locate_max_PSF(bin_len, AveEdge, &pcnt);

	free(shifts);
	free(temp);

	double theta = fabs(atan(*slope));//斜边角度

	// 将LSF的最大值移到中心位置， 并加上汉明窗
	apply_hamming_window((unsigned short)ALPHA, bin_len, (unsigned short)size_x, AveEdge, &pcnt);

	(*lsf) = (double *)malloc(bin_len * sizeof(double));

	//smooth(AveEdge, half_window, bin_len);
	smooth(AveEdge, half_window, bin_len);

	//拷贝LSF
	
	for (i = 0; i < bin_len; i++)
	{
		(*lsf)[i] = AveEdge[i];
		//cout << AveEdge[i] << endl;
	}

	//平滑LSF
	smooth(*lsf, half_window, bin_len);
	//归一化LSF
	double max_lsf = *max_element(*lsf, *lsf + bin_len);
	for (i = 0; i < bin_len; i++)
	{
		(*lsf)[i] = (*lsf)[i]/ max_lsf;

	}

	calculate_esf(bin_len, *esf, *lsf);

	//拷贝ESF
	double max_esf = *max_element(AveTmp, AveTmp + bin_len);
	for (i = 0; i < bin_len; i++)
	{
		AveTmp[i] = AveTmp[i] / max_esf;
		(*esf)[i] = AveTmp[i];
		//cout << (*esf)[i] << endl;
	}


	//ESF平滑

	smooth(*esf, half_window, bin_len);


	max_esf = *max_element(*esf, *esf + bin_len);
	for (i = 0; i < bin_len; i++)
	{
		(*esf)[i] = (*esf)[i] / max_esf;
	}


	//计算10%-90%ESF宽度
	//*esf_len = calculate_esf(*esf, *esf_lsf_x, bin_len, 0.1, 0.9);

	//*esf_len = calculate_esf(*esf, *esf_lsf_x, bin_len, 0.1, 0.9);

	double* lsf_cutted = NULL;

	int lsf_len = bin_len;

	//cut_lsf(AveEdge, &lsf_cutted, lsf_len, bin_len, max_lsf);

	//for (i = 0; i < lsf_len; i++)
	//{
	//	cout << lsf_cutted[i] << endl;
	//}

	//LSF的一半长度作为MTF的长度
	*len = lsf_len / 2;

	tmp = 1.0;
	tmp2 = 1.0 / ((double)lsf_len);

	//double* corr_diff = (double *)malloc(bin_len * sizeof(double));
	//double* diff = (double *)malloc(bin_len * sizeof(double));
	//for (i = 0; i < bin_len; i++)
	//{
	//	corr_diff[i] = 0;
	//}
	//for (i = 0; i < diff_window; i++)
	//{
	//	corr_diff[i] = -1.0/ (double)diff_window;
	//	corr_diff[i+ diff_window+1] = 1.0 / (double)diff_window;
	//}


	/* ftwos ( nx, dx, lsf(x), nf, df, mtf(f) ) */
	//傅里叶变换求MTF
	(void)ftwos(bin_len, tmp, AveEdge, (long)(*len), tmp2, AveTmp);
	//(void)ftwos(lsf_len, tmp, lsf_cutted, (long)(*len),tmp2, AveTmp);

	//(void)ftwos(bin_len, tmp, corr_diff, (long)(*len), tmp2, diff);
	//for (i = 0; i < bin_len; i++)
	//{
	//	//cout << corr_diff[i] << endl;
	//	//cout << -1 / 3 << endl;
	//	cout << diff[i] << endl;
	//}

	

	if (*freq == NULL)
		(*freq) = (double *)malloc((*len) * sizeof(double));
	if (*mtf == NULL)
		(*mtf) = (double *)malloc((*len) * sizeof(double));

	for (i = 0; i<(*len); i++) {
		sfrc = AveTmp[i];
		//(*freq)[i] = ((double)i / (double)size_x) / pixel / cos(theta);// 每个点对应的频率

		(*freq)[i] = ((double)i / (double)(lsf_len)) / pixelc * ALPHA / cos(theta);// 每个点对应的频率
		(*mtf)[i] = (double)(sfrc / AveTmp[0]);// 归一化sfr
		//cout << (*freq)[i] << ":" << (*mtf)[i]<<endl;
	}
	double delta = pixelc / ALPHA;

	for (i = 1; i<(*len); i++) {
		double correct_sinc = sin(M_PI*(*freq)[i]* delta)/(M_PI*(*freq)[i] * delta)*sin(2.0*M_PI*(*freq)[i] * delta * sin(theta/2.0)) / (2*M_PI*(*freq)[i] * delta * sin(theta / 2.0));
		(*mtf)[i] = (*mtf)[i] / correct_sinc;// 矫正CCD的MTF

	}

	//for (i = 1; i<(*len); i++) {
	//	double correct_diff = sin( 2*M_PI*(*freq)[i] * (pixel / ALPHA))/ (2*M_PI*(*freq)[i] * (pixel /ALPHA));
	//	(*mtf)[i] = (*mtf)[i] / correct_diff;// 矫正微分的MTF

	//}
	//for (i = 1; i<(*len); i++) {
	//	double n = 1;
	//	double correct_diff = 0;
	//	while (n <= diff_window)
	//	{
	//		correct_diff += sin(2*M_PI*(*freq)[i] * n*(pixel / ALPHA)) / (2*M_PI*(*freq)[i] * n*(pixel / ALPHA));
	//		n++;
	//	}
	//	(*mtf)[i] = (*mtf)[i] / correct_diff*diff_window;// 矫正窗口微分的MTF

	//}



	for (i = 1; i<(*len); i++) {
		double correct_diff = sin(M_PI*(*freq)[i] * (pixelc / ALPHA)) / (M_PI*(*freq)[i] * (pixelc / ALPHA));
		(*mtf)[i] = (*mtf)[i] / correct_diff;// 矫正有限元微分的MTF

	}
	
	for (i = 1; i<(*len); i++) {
		double correct_smooth = sin(((double) half_window * 2 + 1) * M_PI*(*freq)[i] * pixelc / ALPHA) / sin(M_PI*(*freq)[i] * pixelc / ALPHA) / ((double)half_window * 2 + 1);
		if ((*freq)[i] < ALPHA / pixelc / (double)(half_window * 2 + 1)) {
			(*mtf)[i] = (*mtf)[i] / correct_smooth ;// 矫正平滑
			
		}
		else
			break;
	}


	//for (i = 0; i<(*len); i++) {
	//	(*freq)[i] = (*freq)[i] /cos(theta);// 频域缩放
	//	cout << (*freq)[i] << ":" << (*mtf)[i] << endl;
	//}


	/* Free */
	free(AveEdge);
	free(AveTmp);
	free(lsf_cutted);
	//free(corr_diff);
	//free(diff);
	*nrows = size_y;
	*pcnt2 = pcnt;

	return(0);
}
/****************************************************************************/

void cut_lsf(double *lsf_temp, double **lsf, int &lsf_len, int& bin_len, double max)
{
	int left = bin_len/2;
	int right = bin_len / 2;
	while (lsf_temp[--left] > max*0.3);
	while (lsf_temp[++right] > max*0.3);
	lsf_len = 5 * (right - left);

	if (lsf_len > bin_len)
		lsf_len = bin_len;

	*lsf = (double *)malloc(lsf_len * sizeof(double));
	for (int i = 0; i < lsf_len; i++)
	{
		(*lsf)[i] = lsf_temp[(bin_len - lsf_len) / 2+i];
	}
}

//平滑函数
void smooth(double *value, int half_window, int len)
{
	int i, j;
	double *temp = (double *)malloc((len-2*half_window) * sizeof(double));
	double sum;
	for (i = half_window; i < len - half_window - 1; i++)
	{
		sum = 0;
		for (j = 0; j <= 2 * half_window; j++) {
			sum = sum + value[i - half_window + j];
		}
		temp[i - half_window] = sum / (2 * half_window + 1);
	}

	for (i = half_window; i < len - half_window - 1; i++) {
		value[i] = temp[i - half_window];
	}

	free(temp);
}


void gamma_correct(double *value, double gamma, int len)
{

	for (int i = 0; i < len; i++)
	{
		value[i] = pow(value[i], 1/ gamma);

		//if(value[i]<0.5)
		//	value[i] = pow(value[i], 1 / gamma);
		//else
		//	value[i] = pow(value[i], gamma);
	}


}

/*****************************************************************************/
//计算ESF宽度
double calculate_esf(double *esf, double *esf_lsf_x, int len, double esf1, double esf2)
{
	//for (int i = 0; i < len; i++)
	//{
	//	cout << esf[i] << endl;
	//}
	int beg = 0, end = len - 1;
	while (esf[++beg] < esf1);
	while (esf[--end] > esf2);

	int fit_len = end - beg + 1 + 2;
	double esf_len1, esf_len2;
	double *esf_x = (double *)malloc(fit_len * sizeof(double));
	double *esf_y = (double *)malloc(fit_len * sizeof(double));
	for (int i = 0; i < fit_len; i++)
	{
		esf_x[i] = esf[beg - 1 + i];
		esf_y[i] = esf_lsf_x[beg - 1 + i];
		//cout << esf_x[i] << endl;
	}
	for (int i = 1; i < fit_len-1; i++)
	{
		if (esf_x[i] == esf_x[i - 1])
			esf_x[i] = (esf_x[i - 1] + esf_x[i + 1]) / 2;
		//cout << esf_x[i] << "," << esf_y[i] << endl;
		//cout << esf_x[i] << endl;
		//cout << esf_y[i] << endl;
	}
	//for (int i = 0; i < fit_len ; i++)
	//{


	//	cout << esf_x[i] << endl;
	//	//cout << esf_y[i] << endl;
	//}

	SplineSpace::Spline sp(esf_x, esf_y, fit_len);

	sp.SinglePointInterp(esf1, esf_len1);
	sp.SinglePointInterp(esf2, esf_len2);
	//cout << esf_len1 << "," << esf_len2 << endl;

	free(esf_x);
	free(esf_y);
	return fabs(esf_len1 - esf_len2);

}

/****************************************************************/
// 寻找质心
// shifts --> 当前行与中心行的距离
// temp --> 当前行的质心与中心行的质心的距离
// offset --> 中心行质心的位置
unsigned short locate_centroids(double *farea, double *temp,
	double *shifts,
	unsigned short size_x, unsigned short size_y,
	double *offset, bool hammingfilter) {
	unsigned long i, j;
	double dt, dt1, dt2;
	double hamming=0;
	
	// 求质心C
	for (j = 0; j < size_y; j++) {
		dt = 0.0;
		dt1 = 0.0;
		for (i = 0; i < size_x - 1; i++) {
			//dt2 = farea[(j*(long)size_x) + (i + 1)] - farea[(j*(long)size_x) + i];// 计算每个点的导数
			dt2 = farea[(j*(long)size_x) + i];
			if (hammingfilter) {
				hamming = 0.54 + 0.46 * cos(M_PI * ((double)i - ((double)size_x / 2)) / ((double)size_x / 2));
				dt2 = dt2 * hamming;
			}
			dt += dt2 * (double)i;
			dt1 += dt2;
		}
		shifts[j] = dt / dt1;
	}

	
	// 确保线不要太靠近左右边界(> 2 pixels)
	if (shifts[size_y - 1] < 2 || size_x - shifts[size_y - 1] < 2) {
		//printf("WARNING: Edge comes too close to the ROI corners.\n");
		return 5;
	}
	if (shifts[0] < 2 || size_x - shifts[0] < 2) {
		//printf("WARNING: Edge comes too close to the ROI corners.\n");
		return 5;
	}

	j = size_y / 2;
	dt = shifts[j];
	for (i = 0; i < size_y; i++) {
		temp[i] = (double)i - (double)j;// 当前行与中心行的距离 delta Col
		shifts[i] -= dt; // 当前行的质心与中心行的质心的距离delta C
	}
	*offset = dt;// 中心行质心的位置
	return 0;
}
/***************************************************************************/
// 线性回归 y = a + bx
unsigned short fit(unsigned long ndata, double *x, double *y, double *b,
	double *a, double *R2, double *avar, double *bvar)
{
	unsigned long i;
	double t, sxoss, syoss, sx = 0.0, sy = 0.0, st2 = 0.0;
	double ss, sst, sigdat, chi2, siga, sigb;

	*b = 0.0;
	for (i = 0; i < ndata; i++) {
		sx += x[i];
		sy += y[i];
	}
	ss = (double)ndata;
	sxoss = sx / ss;
	syoss = sy / ss;
	for (i = 0; i < ndata; i++) {
		t = x[i] - sxoss;
		st2 += t*t;
		*b += t * y[i];
	}
	*b /= st2;         /* slope  */
	*a = (sy - sx*(*b)) / ss; /* intercept */
	siga = sqrt((1.0 + sx*sx / (ss*st2)) / ss);
	sigb = sqrt(1.0 / st2);
	chi2 = 0.0;
	sst = 0.0;
	for (i = 0; i < ndata; i++) {
		chi2 += SQR(y[i] - (*a) - (*b) * x[i]);
		sst += SQR(y[i] - syoss);
	}
	sigdat = sqrt(chi2 / (ndata - 2));
	siga *= sigdat;
	sigb *= sigdat;
	*R2 = 1.0 - chi2 / sst;
	if (avar != nullptr) *avar = siga;
	if (bvar != nullptr) *bvar = sigb;
	return 0;
}
void polyfit(const double* x, const double* y, size_t length, int poly_n, vector<double>& factor)
{
	factor.resize(poly_n + 1, 0);
	int i, j;
	//double *tempx,*tempy,*sumxx,*sumxy,*ata;
	std::vector<double> tempx(length, 1.0);

	std::vector<double> tempy(y, y + length);

	std::vector<double> sumxx(poly_n * 2 + 1);
	std::vector<double> ata((poly_n + 1) * (poly_n + 1));
	std::vector<double> sumxy(poly_n + 1);
	for (i = 0; i < 2 * poly_n + 1; i++) {
		for (sumxx[i] = 0, j = 0; j < length; j++)
		{
			sumxx[i] += tempx[j];
			tempx[j] *= x[j];
		}
	}
	for (i = 0; i < poly_n + 1; i++) {
		for (sumxy[i] = 0, j = 0; j < length; j++)
		{
			sumxy[i] += tempy[j];
			tempy[j] *= x[j];
		}
	}
	for (i = 0; i < poly_n + 1; i++)
		for (j = 0; j < poly_n + 1; j++)
			ata[i * (poly_n + 1) + j] = sumxx[i + j];
	gauss_solve(poly_n + 1, ata, factor, sumxy);

	////计算拟合后的数据并计算误差
	//fitedYs.reserve(length);
	//calcError(&x[0], &y[0], length, this->ssr, this->sse, this->rmse, isSaveFitYs);

}
void gauss_solve(int n, std::vector<double>& A, std::vector<double>& x, std::vector<double>& b)
{
	gauss_solve(n, &A[0], &x[0], &b[0]);
}


void gauss_solve(int n, double* A, double* x, double* b)
{
	int i, j, k, r;
	double max;
	for (k = 0; k < n - 1; k++)
	{
		max = fabs(A[k * n + k]); /*find maxmum*/
		r = k;
		for (i = k + 1; i < n - 1; i++) {
			if (max < fabs(A[i * n + i]))
			{
				max = fabs(A[i * n + i]);
				r = i;
			}
		}
		if (r != k) {
			for (i = 0; i < n; i++)         /*change array:A[k]&A[r] */
			{
				max = A[k * n + i];
				A[k * n + i] = A[r * n + i];
				A[r * n + i] = max;
			}
		}
		max = b[k];                    /*change array:b[k]&b[r]     */
		b[k] = b[r];
		b[r] = max;
		for (i = k + 1; i < n; i++)
		{
			for (j = k + 1; j < n; j++)
				A[i * n + j] -= A[i * n + k] * A[k * n + j] / A[k * n + k];
			b[i] -= A[i * n + k] * b[k] / A[k * n + k];
		}
	}

	for (i = n - 1; i >= 0; x[i] /= A[i * n + i], i--)
		for (j = i + 1, x[i] = b[i]; j < n; j++)
			x[i] -= A[i * n + j] * x[j];
}
double getY(const double x, const vector<double> factor)
{
	double ans(0);
	for (size_t i = 0; i < factor.size(); ++i)
	{
		ans += factor[i] * pow((double)x, (int)i);
	}
	return ans;
}

/****************************************************************************/
unsigned short check_slope(double slope, unsigned short *size_y, int *numcycles, double mincyc, int errflag)
{
	double absslope;

	absslope = fabs(slope);

	if (*numcycles <= 0)
		(*numcycles) = (int)((*size_y)*absslope);

	
	if (absslope < mincyc / (double)(*size_y)) {
		if (errflag == 1) {
			/*printf("WARNING: Edge angle (%f) is so shallow it needs\n",
				atan(slope) * 180 / M_PI);
			printf("  %d lines of data (%.1f cycles) for accurate results\n",
				(int)ceil(mincyc / absslope), mincyc);*/
			return 0;
		}
		else
			return 1;
	}

	if (absslope > (double)(1.0 / 4.0)) {
		int rows_per_col;
		double bestcycle, x;

		if (absslope > (double)(5.0 / 4.0)) {
			/*printf("ERROR: Edge angle (%f) is too large\n",
				atan(slope) * 180 / M_PI);*/
			return 1;
		}

		rows_per_col = (int)floor(1 / absslope + 0.5);
		x = fabs(1 / (double)rows_per_col - absslope);
		bestcycle = 4 * rows_per_col*x*ceil(1.0 / x / (double)rows_per_col / (double)rows_per_col - 1.0);
		if ((int)ceil(mincyc*bestcycle) > *size_y) {
			if (errflag == 1) {
				/*printf("WARNING: Edge angle (%f) will reduce SFR accuracy\n",
					atan(slope) * 180 / M_PI);
				printf("   if %g * %f = %d lines of data are not in ROI\n\n",
					mincyc, bestcycle,
					(int)ceil(mincyc*bestcycle));*/
				return 0;
			}
			else
				return 1;
		}
	}


	if (((*numcycles) / absslope) <= *size_y)
		*size_y = (unsigned short)((*numcycles) / absslope);

	return 0;
}

/*****************************************************************************/


unsigned short bin_to_regular_xgrid(unsigned short alpha,
	double *edgex, double *Signal,
	double *AveEdge, long *counts,
	unsigned short size_x,
	unsigned short size_y)
{
	long i, j, k, bin_number;
	long bin_len;
	int nzeros;
	// 进行超采样， 把所有数据超采样到长度为size_x * alpha（4）的一行数据中
	bin_len = size_x * alpha;

	for (i = 0; i<bin_len; i++) {
		AveEdge[i] = 0;
		counts[i] = 0;
	}
	for (i = 0; i<(size_x*(long)size_y); i++) {
		bin_number = (long)floor((double)alpha*edgex[i]);// 点离中心的距离*alpha（4）
		if (bin_number >= 0) // 下界之内
		{
			if (bin_number <= (bin_len - 1)) // 上界之内
			{
				AveEdge[bin_number] = AveEdge[bin_number] + Signal[i];// 累加到对应距离内的容器
				counts[bin_number] = (counts[bin_number]) + 1;// 计数器加一
			}
		}
	}

	//判断结果中有无0
	nzeros = 0;
	for (i = 0; i<bin_len; i++) {
		j = 0;
		k = 1;
		if (counts[i] == 0) {
			nzeros++;
			if (i == 0) {
				while (!j) {
					if (counts[i + k] != 0) {
						AveEdge[i] = AveEdge[i + k] / ((double)counts[i + k]);
						j = 1;
					}
					else k++;
				}
			}
			else {
				while (!j && ((i - k) >= 0)) {
					if (counts[i - k] != 0) {
						AveEdge[i] = AveEdge[i - k];   /* Don't divide by counts since it already happened in previous iteration */
						j = 1;
					}
					else k++;
				}
				if ((i - k) < 0) {
					k = 1;
					while (!j) {
						if (counts[i + k] != 0) {
							AveEdge[i] = AveEdge[i + k] / ((double)counts[i + k]);
							j = 1;
						}
						else k++;
					}
				}
			}
		}
		else
			AveEdge[i] = (AveEdge[i]) / ((double)counts[i]);
	}

	if (nzeros > 0) {
		//printf("\nWARNING: %d Zero counts found during projection binning.\n", nzeros);
		//printf("The edge angle may be large, or you may need more lines of data.\n\n");
	}
	return nzeros;
}
/*****************************************************************************/

void calculate_derivative_new(unsigned int len, double *AveTmp, double *AveEdge,
	double *centroid, int separation)
{
	int i;
	int j;
	double dt, dt1;
	double t1, t2;
	
	dt = 0.0;
	dt1 = 0.0;

	for (i = 0; i< len; i++)
		AveTmp[i] = AveEdge[i];

	for (i = separation; i< len - separation; i++) {
		t1 = 0.0;
		t2 = 0.0;
		for (j = 1; j <= separation; j++) {
			t1 += AveTmp[i + j];
			t2 += AveTmp[i - j];
		}
		AveEdge[i] = (t1 - t2)/separation;
		if (AveEdge[i] < 0)
			AveEdge[i] = 0;
		dt += AveEdge[i] * (double)i;
		dt1 += AveEdge[i];
	}
	for (i = 0; i < separation; i++) {
		AveEdge[i] = AveEdge[separation];
	}
	for (j = len-separation; j < len; j++) {
		AveEdge[j] = AveEdge[len - separation -1];
	}
	*centroid = dt / dt1;

}

void calculate_derivative_old(unsigned int len, double *AveTmp, double *AveEdge,
	double *centroid, int separation)
{
	unsigned long i;
	double dt, dt1;

	dt = 0.0;
	dt1 = 0.0;

	for (i = 0; i< len; i++)
		AveTmp[i] = AveEdge[i];

	for (i = 1; i< len - separation; i++) {
		/* Not wasting time with division by 2 since constant factors don't
		change SFR computation */
		AveEdge[i] = fabs(AveTmp[i + separation] - AveTmp[i - 1]);
		if (separation == 1)
			AveEdge[i] /= 2.0;
		dt += AveEdge[i] * (double)i;
		dt1 += AveEdge[i];
	}

	*centroid = dt / dt1;

	AveEdge[0] = AveEdge[1];
	if (separation == 1) AveEdge[len - 1] = AveEdge[len - 2];
}

void calculate_esf(unsigned int len, double *AveTmp, double *AveEdge)
{
	unsigned long i;
	double dt, dt1;

	dt = 0.0;
	dt1 = 0.0;
	double min_lsf = *min_element(AveEdge, AveEdge + len);

	for (i = 0; i< len; i++)
		AveEdge[i] = AveEdge[i]-min_lsf;

	AveTmp[0] = 0;
	for (i = 1; i< len ; i++) {
		/* Not wasting time with division by 2 since constant factors don't
		change SFR computation */
		AveTmp[i] = fabs(AveEdge[i] + AveTmp[i - 1]);
	}


	
}

/*****************************************************************************/
void locate_max_PSF(unsigned int len,
	double *AveEdge, long *pcnt2)
{
	unsigned long i;
	double dt = 0.0, dt_new = 0.0;
	long left = -1L, right = -1L;

	/* find maximim value in Point Spread Function array */
	for (i = 0; i<len; i++) {
		dt_new = fabs(AveEdge[i]);
		if (dt_new > dt) {
			(*pcnt2) = (long)i;
			dt = dt_new;
		}
	}
	/* find leftmost and rightmost occurrence of maximum */
	for (i = 0; i<len; i++) {
		dt_new = fabs(AveEdge[i]);
		if (dt_new == dt) {
			if (left < 0) left = i;
			right = i;
		}
	}
	/* find centre of maxima */
	(*pcnt2) = (right + left) / 2;
}

/*****************************************************************************/
void apply_hamming_window(unsigned short alpha,
	unsigned int oldlen,
	unsigned short newxwidth,
	double *AveEdge, long *pcnt2)
{
	long i, j, k, begin, end, edge_offset;
	double sfrc;
	
	/* 移动LSF最大值到中心 */
	edge_offset = (*pcnt2) - (oldlen / 2);
	if (edge_offset != 0) {
		if (edge_offset < 0) {
			for (i = oldlen - 1; i > -edge_offset - 1; i--)
				AveEdge[i] = (AveEdge[i + edge_offset]);
			for (i = 0; i < -edge_offset; i++)
				AveEdge[i] = 0.00; /* last operation */
		}
		else {
			for (i = 0; i < oldlen - edge_offset; i++)
				AveEdge[i] = (AveEdge[i + edge_offset]);
			for (i = oldlen - edge_offset; i < oldlen; i++)
				AveEdge[i] = 0.00;
		}
	}
	/* 为LSF加Hanming窗 */
	begin = (oldlen / 2) - (newxwidth*alpha / 2);
	if (begin < 0) begin = 0;
	end = (oldlen / 2) + (newxwidth*alpha / 2);
	if (end > oldlen)  end = oldlen;
	for (i = 0; i< begin; i++)
		AveEdge[i] = 0.0;
	for (i = end; i< oldlen; i++)
		AveEdge[i] = 0.0;

	for (i = begin, j = -newxwidth*alpha / 2; i < end; i++, j++) {
		sfrc = 0.54 + 0.46*cos((M_PI*(double)j) / (newxwidth*alpha / 2));
		AveEdge[i] = (AveEdge[i])*sfrc;
	}

	if (begin != 0) /* Shift LSF to begin at index 0 (rather than begin) */
		for (k = 0, i = begin; k<newxwidth*alpha; i++, k++)
			AveEdge[k] = AveEdge[i];

}

/*****************************************************************************/
/* DFT                                            */
unsigned short ftwos(long number, double dx, double *lsf,
	long ns, double ds, double *sfr)
{
	double a, b, twopi, g;
	long i, j;

	twopi = 2.0 * M_PI;
	for (j = 0; j < ns; j++) {
		g = twopi * dx * ds * (double)j;
		for (i = 0, a = 0, b = 0; i < number; i++) {
			a += lsf[i] * cos(g * (double)(i));
			b += lsf[i] * sin(g * (double)(i));
		}
		sfr[j] = sqrt(a * a + b * b);
	}
	return 0;
}

void reduce_pixelization(double* esf, int len)
{
	double m, ml, mr;
	double* index;
	double max = 0.0;
	m = std::accumulate(esf, esf + len, 0.0) / len;
	int gap = 10;
	int i;
	for (i = gap; i < len - gap; i++)
	{
		index = esf + i;
		ml = std::accumulate(index - gap, index, 0.0) / gap;
		mr = std::accumulate(index, index + gap, 0.0) / gap;
		if (*index > ml && *index > mr && *index > m)
		{
			max = *index;
			break;
		}
	}

	for (int j = i; j < len; j++)
	{
		esf[j] = max;
	}


}
SLANDEDCROSS_END