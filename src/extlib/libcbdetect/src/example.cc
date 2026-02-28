#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"
#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>
using namespace std;
using namespace std::chrono;
vector<cv::Point2d> detect_display_fiducial(cv::Mat img,cv::Rect* roisPtr = nullptr,int roiNums =2) {
	vector<cv::Point2d> result;
	int errCode = 0;
	cbdetect::Corner corners;
	cbdetect::Params params;
	params.corner_type = cbdetect::SaddlePoint;
	params.show_debug_image = false;
	params.find_corners_reiszed = false;
	if (roisPtr == nullptr) {
		try {
			cbdetect::find_corners(img, corners, params);
			result = corners.p;
		}
		catch (...) {
			return result;
		}
	}
	else {
		for (int i = 0; i < roiNums; i++) {
			cv::Rect rect = *roisPtr;
			cv::Mat roiimg = img(rect);
			try {
				if (roiimg.data) {
					cbdetect::find_corners(roiimg, corners, params);
					for (int j = 0; j < corners.p.size(); j++) {
						cv::Point2d pt;
						pt.x = corners.p.at(j).x + rect.x;
						pt.y = corners.p.at(j).y + rect.y;
						result.push_back(pt);
					}
				}
			}
			catch (...) {
				return result;
			}
			roisPtr++;
		}
	}
	return result;
}
void detect(const char* str, cbdetect::CornerType corner_type) {
  cbdetect::Corner corners;
  std::vector<cbdetect::Board> boards;
  cbdetect::Params params;
  params.corner_type = corner_type;
  params.show_debug_image =false;
  params.find_corners_reiszed = false;
  cv::Mat img = cv::imread(str, cv::IMREAD_COLOR);
  cv::Rect rect;
  rect.x = 1738;
  rect.y = 237;
  rect.width = 1000;
  rect.height = 300;
 // img = img(rect);
  auto t1 = high_resolution_clock::now();
  cbdetect::find_corners(img, corners, params);
  auto t2 = high_resolution_clock::now();
  cbdetect::plot_corners(img, corners);
  auto t3 = high_resolution_clock::now();
  cbdetect::boards_from_corners(img, corners, boards, params);
  auto t4 = high_resolution_clock::now();
  printf("Find corners took: %.3f ms\n", duration_cast<microseconds>(t2 - t1).count() / 1000.0);
  printf("Find boards took: %.3f ms\n", duration_cast<microseconds>(t4 - t3).count() / 1000.0);
  printf("Total took: %.3f ms\n", duration_cast<microseconds>(t2 - t1).count() / 1000.0 + duration_cast<microseconds>(t4 - t3).count() / 1000.0);
  cbdetect::plot_boards(img, corners, boards, params);
}

int main(int argc, char* argv[]) {
	printf("chessboards...");
	//detect("../../example_data/e2.png", cbdetect::SaddlePoint);
	//detect("d:/sample/display_fiducials.bmp", cbdetect::SaddlePoint);
	cv::Rect rois[2];
	cv::Rect rect;
	rect.x = 1738;
	rect.y = 237;
	rect.width = 1000;
	rect.height = 300;
	rois[0] = rect;

	cv::Rect rect2(396, 981, 272, 505);
	rois[1] = rect2;
	cv::Mat img = cv::imread("d:/sample/display_fiducials.bmp", cv::IMREAD_COLOR);
	vector<cv::Point2d> res = detect_display_fiducial(img);//, rois);
	for (int i = 0; i <res.size(); ++i) {
		cv::circle(img, res[i], 3, cv::Scalar(0, 0, 255), -1);
	}
	printf("deltilles...");
	//detect("../../example_data/e6.png", cbdetect::MonkeySaddlePoint);
	return 0;
}