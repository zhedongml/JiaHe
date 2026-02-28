#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dmtx.h>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"


std::string decodeMatrixRegion(cv::Mat img ,int startx=0,int starty=0,int width=-1,int height=-1) {
	cv::Mat roiimg;
	std::string result="";
	if (width == -1 || height == -1) {
		roiimg = img;
	}
	else {
		cv::Rect rect(startx, starty, width, height);
	     roiimg = img(rect).clone();
	}
	DmtxImage* decodeimg;
	DmtxDecode* dec;
	DmtxRegion* reg;
	DmtxMessage* msg;
	DmtxPackOrder type = DmtxPack8bppK;
	switch (roiimg.type()) {
	case CV_8UC1:
		type = DmtxPack8bppK;
		break;
	case CV_16UC1:
		type = DmtxPack8bppK;
		break;
	case CV_8UC3:
		type = DmtxPack24bppBGR;
		break;
	}
	
	decodeimg = dmtxImageCreate(roiimg.data, roiimg.cols, roiimg.rows, type);

	dec = dmtxDecodeCreate(decodeimg, 1);
	if (dec == NULL) {
		return "";
	}

	reg = dmtxRegionFindNext(dec, NULL);
	if (reg != NULL) {
		msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
		if (msg != NULL) {
		    result=((char*)msg->output);
			dmtxMessageDestroy(&msg);
		}
		dmtxRegionDestroy(&reg);
	}
	dmtxDecodeDestroy(&dec);
	dmtxImageDestroy(&decodeimg);
	return result;
}
int main1()
{
	cv::Mat img = cv::imread("d:\\sample\\test1\\datamatrix.bmp",-1);
	std::string aa = decodeMatrixRegion(img , 2000, 900, 200,200);
	std::cout <<aa;
	std::cin >> aa;
	return 1;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
