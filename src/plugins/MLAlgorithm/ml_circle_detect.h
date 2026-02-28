#pragma once
#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <math.h>
#include <vector>
using namespace std;
struct CirclePos
{
    int x;
    int y;
    int r;
};
class Ml_Circle_detect
{
public:
    vector<cv::Vec3f> detectCircle(cv::Mat img, int startx, int starty, int width, int height,bool isRight =true,int radius=140);
private:
    vector<int> getAllRadius(int minR, int maxR, int step);
    cv::Mat max_min_value_filter(cv::Mat image, int ksize = 3, int mode = 1);
    cv::Mat makeAnnulusKernel(int outer_radius, int annulus_width);
    CirclePos getTopCircle(cv::Mat image, int radius);
    bool isDebug = false;
    int _ANNULUS_WIDTH = 85;
};
