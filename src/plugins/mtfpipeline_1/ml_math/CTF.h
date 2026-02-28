#pragma once

#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <algorithm>
#include <functional>
#include <numeric>

using namespace std;
using namespace cv;

class CTF
{
public:
	CTF();
	~CTF();
	CTF(Mat img);
	double get_frequency();
	double get_contrast();
private:
	int row;
	int col;
	Mat img_gray;
	int flag;
	vector<vector<double>> data;
	void freqAndContrast(vector<double> per, double &freq, double &contrast);
	double frequency;
	double contrast_value;
	double angle;
	void getPerCTF();
};

