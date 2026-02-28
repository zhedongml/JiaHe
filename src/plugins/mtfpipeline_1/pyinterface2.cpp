#include "pipeline.h"

extern "C" {
	__declspec(dllexport) void releasePipeLine(PipeLine* mtfPipelinec) {
		if(mtfPipelinec!=NULL)
		  delete  mtfPipelinec;
	}
	__declspec(dllexport) PipeLine* pipeLineNew() {
		 return new PipeLine();
	 }
	__declspec(dllexport) int get_bin_len_c(PipeLine* mtfPipelinec) {
		return mtfPipelinec->get_bin_len();
	}
	__declspec(dllexport) double* get_freq_c(PipeLine* mtfPipelinec) {
		return mtfPipelinec->get_freq();
	};

	__declspec(dllexport) double* get_mtf_c(PipeLine* mtfPipelinec) {
		return mtfPipelinec->get_mtf();
	};
}
extern "C" __declspec(dllexport) double* get_esf_lsf_x_c(PipeLine * mtfPipelinec) {
	return mtfPipelinec->get_esf_lsf_x();
};

extern "C"  __declspec(dllexport) double* get_esf_c(PipeLine * mtfPipelinec) {
	return mtfPipelinec->get_esf();
};

extern "C" __declspec(dllexport) double* get_lsf_c(PipeLine * mtfPipelinec) {
	return mtfPipelinec->get_lsf();
};
extern "C" __declspec(dllexport) int  culc_mtf_c(PipeLine * mtfPipelinec,cv::Mat & roi, int roiType)
{
    CORE::ML_MTF_TYPE type;
	if (roiType == 1) {
        type = CORE::SLANT;
	}
	else {
        type = CORE::CROSS;
	}
	return mtfPipelinec->culc_mtf(roi, type);
};
extern "C"  __declspec(dllexport) int  culcimg_mtf_c(PipeLine * mtfPipelinec,char* roiSrc, int roiType)
{
	cv::Mat img = imread(roiSrc, -1);
	if (img.data) {
		return culc_mtf_c(mtfPipelinec,img, roiType);
	}
	return -1;
};
extern "C"  __declspec(dllexport) int  culcimgrect_mtf_c(PipeLine * mtfPipelinec,char* roiSrc, int roiType, int startx, int starty, int width, int height)
{
	cv::Mat img = imread(roiSrc, -1);
	cv::Rect rect(startx, starty, width, height);
	img = img(rect);
	if (img.data) {
		return culc_mtf_c(mtfPipelinec, img, roiType);
	}
	return -1;
};
extern "C"  __declspec(dllexport) void SetPixelValue_c(PipeLine * mtfPipelinec,double value)
{
	mtfPipelinec->SetPixelValue(value);
}

extern "C" __declspec(dllexport) void set_freq_unit_c(PipeLine * mtfPipelinec,int unit, double focalLength, double coefficient) {
    CORE::ML_FREQ_UNIT frequnit = CORE::ML_FREQ_UNIT::lp_deg;
	if (unit == 0) {
        frequnit = CORE::ML_FREQ_UNIT::lp_mm;
	}
	mtfPipelinec->set_freq_unit(frequnit, focalLength, coefficient);
}
extern "C"  __declspec(dllexport) double getMtfByFreq_c(PipeLine * mtfPipelinec,double freq)
{
	return mtfPipelinec->getMtfByFreq(freq);
}