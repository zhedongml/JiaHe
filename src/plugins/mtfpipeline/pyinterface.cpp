#include "pipeline.h"
#include "time.h"
#include "autoroi.h"
#include<thread>
long long start = 1636691443;
PipeLine* mtfPipeline = new PipeLine;
//extern "C" bool __declspec(dllexport) configProcessor(char* configFilePath);
bool licenceValidate = false;
typedef struct ROIInfo{
	int startx=0;
	int starty=0;
	int width=0;
	int height=0;
};

int findLastIndex(string& str, char x)
{
	// Traverse from right
	for (int i = str.length() - 1; i >= 0; i--)
		if (str[i] == x)
			return i;

	return -1;
}
extern "C" {
 int __declspec(dllexport)   configProcessor(const char* configFilePath) {
	 time_t myt = time(NULL);
	 //if (myt - start > 8640000)
		// return -3;
	 string configpath = configFilePath;
	 int pos = findLastIndex(configpath, '/');
	 string dir = configpath.substr(0, pos);
	return mtfPipeline->configProcessor(configFilePath);
};
 __declspec(dllexport) int get_bin_len() {
	 return mtfPipeline->get_bin_len();
 }
__declspec(dllexport) double* get_freq() {
	return mtfPipeline->get_freq();
};

 __declspec(dllexport) double*  get_mtf() {
	return mtfPipeline->get_mtf();
};
}
extern "C" __declspec(dllexport) void set_mtf_interp(int num) {
	return mtfPipeline->set_mtf_interp(num);
};
extern "C" __declspec(dllexport) double* get_freq_interp() {
	return mtfPipeline->get_freq_interp();
};
extern "C" __declspec(dllexport) double* get_mtf_interp() {
	return mtfPipeline->get_mtf_interp();
};

extern "C" __declspec(dllexport) double*  get_esf_lsf_x() {
	return mtfPipeline->get_esf_lsf_x();
};

extern "C"  __declspec(dllexport) double* get_esf() {
	return mtfPipeline->get_esf();
};

extern "C" __declspec(dllexport) double*  get_lsf() {
	return mtfPipeline->get_lsf();
};
extern "C" __declspec(dllexport) int  culc_mtf(cv::Mat& roi, int roiType)
{
	time_t myt = time(NULL);
	//if (myt - start > 8640000)
		//return -2;
	if (!licenceValidate)
		return -2;
	ML_MTF_TYPE type ;
	if (roiType == 1) {
		type = SLANT;
	}
	else {
		type = CROSS;
	}
	return mtfPipeline->culc_mtf(roi, type);
};
extern "C"  __declspec(dllexport) int  culcimg_mtf(char* roiSrc, int roiType)
{
	cv::Mat img = imread(roiSrc, -1);
	if (img.data) {
		return culc_mtf(img, roiType);
	}
	return -1;
};
extern "C"  __declspec(dllexport) int  culcimgrect_mtf(char* roiSrc, int roiType,int startx,int starty,int width,int height)
{
	cv::Mat img = imread(roiSrc, -1);
	cv::Rect rect(startx, starty, width, height);
	img = img(rect);
	if (img.data) {
		return culc_mtf(img, roiType);
	}
	return -1;
};
extern "C"  __declspec(dllexport) void SetPixelValue(double value)
{
	mtfPipeline->SetPixelValue(value);
}

extern "C" __declspec(dllexport) void set_freq_unit(int unit, double focalLength, double coefficient) {
	ML_FREQ_UNIT frequnit = ML_FREQ_UNIT::lp_deg;
	if (unit == 0) {
		frequnit = ML_FREQ_UNIT::lp_mm;
	}
	mtfPipeline->set_freq_unit(frequnit, focalLength, coefficient);
}
extern "C"  __declspec(dllexport) double getMtfByFreq(double freq)
{
	return mtfPipeline->getMtfByFreq(freq);
}
extern "C" cv::Mat ConverttoCV(MLFrame frame)
{
	cv::Mat m;
	switch (frame.format)
	{
	case MLPixelFormat::kMLMono8:
	{
		m = cv::Mat(frame.height, frame.width, CV_8UC1, frame.buffer_ptr);
		break;
	}
	case MLPixelFormat::kMLMono12:
	case MLPixelFormat::kMLMono16:
	{
		m = cv::Mat(frame.height, frame.width, CV_16UC1, frame.buffer_ptr);

		break;
	}
	case MLPixelFormat::kMLRGB24:
	{
		m = cv::Mat(frame.height, frame.width, CV_8UC3, frame.buffer_ptr);
		break;
	}
	default:
		break;
	}

	//delete[](frame.buffer_ptr);  //out of memory

	//frame.buffer_ptr = NULL;

	return m;
}
extern "C"  __declspec(dllexport) int  culcPyMatRect_mtf(MLFrame frame, int roiType, int startx, int starty, int width, int height)
{
	cv::Mat img = ConverttoCV(frame);
	if (startx<0 || starty<0 || startx + width>img.cols || starty + height>img.rows) {
		printf("roi error");
		return -1;
	}
	
	cv::Rect rect(startx, starty, width, height);
	img = img(rect);
	if (img.data) {
		return culc_mtf(img, roiType);
	}
	return -1;
};
extern "C"  __declspec(dllexport) ChromaticAberration*  getChromaticAberration(MLFrame frame1, MLFrame frame2, MLFrame frame3) {
	cv::Mat rawimg1 = ConverttoCV(frame1);
	cv::Mat rawimg2 = ConverttoCV(frame2);
	cv::Mat rawimg3 = ConverttoCV(frame3);
	AutoROI autroi;
	ChromaticAberration* result = autroi.autoChromaticAberration(rawimg1, rawimg2, rawimg3);
	return result;
};
extern "C"  __declspec(dllexport) ROIInfo * getROIsOfPyMat(MLFrame frame, int type, int count, int roiwidth, int roiheight, int offset, bool isThroFocus,int roiDir)
{
	cv::Mat rawimg = ConverttoCV(frame);
	cv::Mat img = rawimg;
	if (rawimg.type() == CV_16UC1) {
		//convertScaleAbs(rawimg, img);
		//rawimg.convertTo(img, CV_8UC1);
	}
	AutoROI autroi;
	std::vector<Rect> autoroiVec = autroi.findAutoROIs(img, type, count, roiwidth, roiheight, offset, isThroFocus,roiDir);
	ROIInfo* autoRoiArry = new ROIInfo[4];
	int num = autoroiVec.size() <= 4 ? autoroiVec.size() : 4;
	for (int i = 0; i < num; i++) {
		autoRoiArry[i].startx = autoroiVec.at(i).x;
		autoRoiArry[i].starty = autoroiVec.at(i).y;
		autoRoiArry[i].width = autoroiVec.at(i).width;
		autoRoiArry[i].height = autoroiVec.at(i).height;
	}
	return autoRoiArry;
};

void  getROIsOfPyMatNewThread(MLFrame frame, int type, int count, int roiwidth, int roiheight, int offset, bool isThroFocus, int roiDir, ROIInfo* autoRoiArry)
{
	/*std::ostringstream oss;
	oss << std::this_thread::get_id() << std::endl;
	printf("threadid %s\n", oss.str().c_str());*/
	cv::Mat rawimg = ConverttoCV(frame);
	cv::Mat img = rawimg;
	if (rawimg.type() == CV_16UC1) {
		//convertScaleAbs(rawimg, img);
		//rawimg.convertTo(img, CV_8UC1);
	}
	AutoROI autroi;
	std::vector<Rect> autoroiVec = autroi.findAutoROIs(img, type, count, roiwidth, roiheight, offset, isThroFocus, roiDir);
	int num = autoroiVec.size() <= 4 ? autoroiVec.size() : 4;
	for (int i = 0; i < num; i++) {
		autoRoiArry[i].startx = autoroiVec.at(i).x;
		autoRoiArry[i].starty = autoroiVec.at(i).y;
		autoRoiArry[i].width = autoroiVec.at(i).width;
		autoRoiArry[i].height = autoroiVec.at(i).height;
	}
};
void  getROIsOfPyMatNewThread2(MLFrame frame) {

}
extern "C"  __declspec(dllexport) ROIInfo * getROIsOfPyMatMultiThread(MLFrame* frame, int imgcount,int type, int roicount, int roiwidth, int roiheight, int offset, bool isThroFocus, int roiDir)
{
	ROIInfo* autoRoiArry =(ROIInfo*) malloc(imgcount * roicount * sizeof(ROIInfo));
	std::vector<std::thread> threadvec;
	for (int i = 0; i < imgcount; i++) {
		MLFrame fr = frame[i];
		ROIInfo* temp = autoRoiArry+(i * roicount);
		//std::thread th(getROIsOfPyMatNewThread,fr,type, count,roiwidth,roiheight,offset,isThroFocus,roiDir,temp);
		threadvec.push_back(std::thread(getROIsOfPyMatNewThread, fr, type, roicount, roiwidth, roiheight, offset, isThroFocus, roiDir, temp));
	}
	for (int i = 0; i < threadvec.size(); i++) {
		threadvec.at(i).join();
	}
	return autoRoiArry;
};
extern "C" __declspec(dllexport) void releaseAutoRoiMemMultiThread(ROIInfo * roiinfoPtr) {
	if (roiinfoPtr != NULL)
		free(roiinfoPtr);
}
extern "C" __declspec(dllexport) void releaseAutoRoiMem(ROIInfo* roiinfoPtr) {
	if (roiinfoPtr != NULL)
		delete[]  roiinfoPtr;
}
extern "C" __declspec(dllexport) void releaseChromaticAberrationiMem(ChromaticAberration* chroAberPtr) {
	if (chroAberPtr != NULL)
		delete[]  chroAberPtr;
}