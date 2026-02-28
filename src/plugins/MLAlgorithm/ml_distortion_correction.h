#ifndef MLALGORITHM_DISTORTIONCORRECTION_H_
#define MLALGORITHM_DISTORTIONCORRECTION_H_

#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif

#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>


namespace algorithm {

	class ALGORITHM_API MLDistortionCorrection
	{
	public:
		MLDistortionCorrection();
		~MLDistortionCorrection();
		MLDistortionCorrection(cv::Mat instrinsic_matrix, cv::Mat distortion_coefficient, cv::Size img_size);
		void setDistortionMat(cv::Mat instrinsic_matrix, cv::Mat distortion_coefficient);
		void setImageSize(cv::Size img_size);
		cv::Mat GetUndistortedImage(cv::Mat img);
	private:
		cv::Size img_size_;
		cv::Mat map1_, map2_;
		cv::Mat instrinsic_matrix_, distortion_coefficient_;
	};
}

#endif // !MLALGORITHM_DISTORTIONCORRECTION_H_

