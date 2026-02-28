
#ifndef CommonDll_EXPORTS
#define CommonDll_EXPORTS
#endif 
#include <opencv2\imgproc\types_c.h>
#include "CTF.h"

CTF::CTF()
{
}

CTF::~CTF()
{
}


CTF::CTF(Mat img)
{
	Mat img_threshold;
	if (img.channels() == 3)
		cvtColor(img, img_gray, CV_RGB2GRAY);
	row = img_gray.rows;
	col = img_gray.cols;
	//double minv = 0.0, maxv = 0.0;

	//Point minp, maxp;
	//minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);

	threshold(img_gray, img_threshold, 0, 255, CV_THRESH_OTSU);
	//threshold(img_gray, img_gray, (minv + maxv) / 2, 255, THRESH_BINARY);


	//Canny(img_gray, img_gray, 3, 9, 3);
	vector<Vec4i> lines;
	int len = row < col ? row : col;
	HoughLinesP(img_threshold, lines, 1, CV_PI / 180, len / 2, len / 2);
	vector<double> k(lines.size());

	for (int i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		Scalar color = Scalar(rand() % 255, rand() % 255, rand() % 255);
		line(img, Point(l[0], l[1]), Point(l[2], l[3]), color);
		k[i] = double(l[1] - l[3]) / double(l[0] - l[2]);

	}

	double kk = std::accumulate(k.begin(), k.end(), 0.0) / k.size();
	if (isnan(kk))
		kk = INFINITY;

	flag = abs(kk) > 1 ? 1 : 0;//判断方向：1.竖直；0.水平

	angle = abs(kk) > 1 ? abs(abs(atan(kk)) - CV_PI / 2) : abs(atan(kk));

	getPerCTF();

}

void CTF::getPerCTF()
{
	switch (flag)
	{
	case 0:
		for (int i = 0; i < col; i++)
		{
			Mat MatC = img_gray.colRange(i, i + 1);
			Mat _MatC;
			MatC.convertTo(_MatC, CV_64FC1);
			vector<double> temp = (vector<double>)(_MatC.reshape(1, 1));
			data.push_back(temp);
		}
		break;

	case 1:
		for (int i = 0; i < row; i++)
		{
			Mat MatC = img_gray.rowRange(i, i + 1);
			Mat _MatC;
			MatC.convertTo(_MatC, CV_64FC1);
			vector<double> temp = (vector<double>)(_MatC.reshape(1, 1));
			data.push_back(temp);
		}
	}
	vector<double> freq, constrast;
	for (int i = 0; i < data.size(); i++)
	{
		double fr, con;
		freqAndContrast(data[i], fr, con);
		freq.push_back(fr);
		constrast.push_back(con);

	}

	frequency = 1 / (std::accumulate(freq.begin(), freq.end(), 0.0) / freq.size()*cos(angle));

	contrast_value = std::accumulate(constrast.begin(), constrast.end(), 0.0) / constrast.size();

}



void CTF::freqAndContrast(vector<double> per, double &freq, double &contrast)
{
	vector<int> max_index, min_index;
	vector<double> max, min;
	for (int i = 1; i < per.size()-1; i++)
	{
		if (per[i] > per[i - 1] && per[i] > per[i + 1])
		{
			max_index.push_back(i);
			max.push_back(per[i]);
		}

		if (per[i] < per[i - 1] && per[i] < per[i + 1])
		{
			min_index.push_back(i);
			min.push_back(per[i]);
		}
	}

	double max_mean = std::accumulate(max.begin(), max.end(), 0.0) / max.size();
	double min_mean = std::accumulate(min.begin(), min.end(), 0.0) / min.size();
	contrast = (max_mean - min_mean) / (max_mean + min_mean);

	vector<int> dis;
	for (int i = 1; i < max_index.size(); i++)
	{
		dis.push_back(max_index[i] - max_index[i - 1]);
	}
	for (int i = 1; i < min_index.size(); i++)
	{
		dis.push_back(min_index[i] - min_index[i - 1]);
	}

	freq = double( std::accumulate(dis.begin(), dis.end(), 0.0)) / double(dis.size());
}


double CTF::get_frequency()
{
	return frequency;
}


double CTF::get_contrast()
{
	return contrast_value;
}


void ctftest()
{
	double pixel = 3.45e-3;

	Mat img = imread("H.jpg");
	CTF my(img);

	double frequency = my.get_frequency() / pixel;
	double contrast = my.get_contrast();
}