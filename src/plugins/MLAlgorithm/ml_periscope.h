#ifndef MLALGORITHM_MLPERISCOPE_H_
#define MLALGORITHM_MLPERISCOPE_H_

#ifdef ALGORITHM_EXPORTS
#define ALGORITHM_API __declspec(dllexport)
#else
#define ALGORITHM_API __declspec(dllimport)
#endif


#include "ml_cross_center.h"
#include "ml_two_cross.h"

namespace algorithm {

	struct PeriscopeInfo
	{
		cv::Point2f left;
		cv::Point2f right;
	};

	struct PeriscopeError
	{
		cv::Point2f delta_left;
		cv::Point2f delta_right;
		cv::Point2f angle_leftXY;
		cv::Point2f angle_rightXY;
		float angle_left = 0;
		float angle_right = 0;
		float angle_left_to_right = 0;
	};

	enum PeriscopeType
	{
		LEFT = 0,
		RIGHT = 1,
		BOTH = 2
	};

	class ALGORITHM_API MLPeriscope
	{
	public:
		MLPeriscope();
		~MLPeriscope();
		MLPeriscope(float pixel_size, float focal_length, PeriscopeInfo reference, PeriscopeInfo align_error, PeriscopeType type) :
			pixel_size_(pixel_size), focal_length_(focal_length), reference_(reference), align_error_(align_error),  type_(type){};

		// loacte the two cross in periscope.
		void LocatePeriscope(cv::Mat img, int crossNumber);
		PeriscopeInfo GetPeriscopeInfo();
		std::vector<cv::Point2f> GetPeriscopeROIs(int off_set);

		// calculate periscope error according to pixel_size_, focal_length_, reference_, location_ and align_error.
		void CalculatePeriscopeError();
		PeriscopeError GetPeriscopeError();


	private:
		cv::Mat img_;
		int width_, height_;

		int img_divide_;
		cv::Mat left_img_, right_img_;
		PeriscopeInfo reference_, location_, align_error_;
		
		float pixel_size_, focal_length_;
		PeriscopeType type_;
		PeriscopeError periscope_error_;
		MLTwoCross left_cross_, right_cross_;

		void SplitImage(int divide);
		void Combine(cv::Point2f &right);

		float DisConverttoAngle(float x);
		float GetTwoPosDistance(cv::Point2f p1, cv::Point2f p2);

	};

	



}
#endif // !MLALGORITHM_MLPERISCOPE_H_

