
#include "MLPostProcessing.h"

MLPostProcessing::MLPostProcessing()
{
}

MLPostProcessing::~MLPostProcessing()
{
}

cv::Mat MLPostProcessing::ImageRotate(enImageRotate enRotate, cv::Mat src)
{
	cv::Mat result;
	switch (enRotate)
	{
	case enImageRotate::R0:
		result = src.clone();
		break;
	case enImageRotate::R90:
		cv::rotate(src, result, cv::ROTATE_90_CLOCKWISE);
		break;
	case enImageRotate::R180:
		cv::rotate(src, result, cv::ROTATE_180);
		break;
	case enImageRotate::R270:
		cv::rotate(src, result, cv::ROTATE_90_COUNTERCLOCKWISE);
		break;
	default:
		break;
	}
	return result;
}

cv::Mat MLPostProcessing::ImageMirror(enImageMirror enMirror, cv::Mat src)
{
	cv::Mat result;
	switch (enMirror)
	{
	case enImageMirror::Non:
		result = src.clone();
		break;
	case enImageMirror::LeftRight:
		cv::flip(src, result,1);
		break;
	case enImageMirror::UpDown:
		cv::flip(src, result, 0);
			break;
	default:
		break;
	}
	return result;
}

cv::Mat MLPostProcessing::Binning(cv::Mat& src, int bin)
{
	if (src.empty())
	{
		return cv::Mat();
	}
	if (bin < 2)
	{
		return src;
	}

	cv::Mat binning = cv::Mat::zeros(src.rows / bin, src.cols / bin, src.type());

	cv::Mat img_c;
	src.convertTo(img_c, CV_32SC1);
	int w = img_c.cols;
	int h = img_c.rows;


	for (size_t i = 0; i < binning.rows; i++)
	{
		for (size_t j = 0; j < binning.cols; j++)
		{
			unsigned short* data = binning.ptr<unsigned short >(i) + j;

			float sum = 0;
			for (size_t m = 0; m < bin; m++)
			{
				int* p = img_c.ptr<int >(i * bin + m) + j * bin;
				for (size_t n = 0; n < bin; n++)
				{
					sum += p[n];
				}
			}
			float result = sum / (bin * bin);
			data[0] = unsigned short(result);
		}
	}
	return binning;
}

cv::Mat MLPostProcessing::BinningZoom(cv::Mat& src, int bin)
{
	cv::Mat result;
	cv::resize(src, result, src.size() / bin);
	return result;
}