/*
 *      Author: alexanderb
 */

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Harris {
public:
    Harris(Mat img, float k, int filterRange, bool gauss);
	Harris();
	std::vector<cv::Point2d> detect_corners(Mat img, int nms_kernel_size, float sigma, float k = 0.06, float thresh = 0.85, float max_no_corners = -1);

private:
	Mat convertRgbToGrayscale(Mat& img);

	Mat computeIntegralImg(Mat& img);
	Mat meanFilter(Mat& intImg, int range);
	Mat gaussFilter(Mat& img, int range);

	std::vector<Mat> create_Haar_wavelet_xy(float sigma);
		Mat create_corner_response_matrix(Mat img, float sigma, float k, float thresh);
	std::vector<Mat> compute_derivatives(Mat img, Mat filter_x, Mat filter_y);
	Mat local_non_maximal_suppression(Mat corner_resp, int kernel_size = 25);
	std::vector<cv::Point2d> get_max_corners(Mat corner_resp, int max_no_corners = -1);
private:
	Mat m_harrisResponses;
};

