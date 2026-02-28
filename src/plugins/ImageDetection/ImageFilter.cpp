#include "ImageFilter.h"
#include<opencv2/opencv.hpp>
using namespace MLImageDetection;
using namespace cv;
MLImageDetection::ImageFilter::ImageFilter()
{
}

MLImageDetection::ImageFilter::~ImageFilter()
{
}

void MLImageDetection::ImageFilter::GaussianLowPassFilter(const cv::Mat src, cv::Mat& dst)
{
	//1、傅里叶变换

	//，image_output为可显示的频谱图，image_transform为傅里叶变换的复数结果
	cv::Mat image_output, image_transform;
	dftML (src, image_output, image_transform);

	//2、高斯低通滤波

	Mat planes[] = { Mat_<float>(image_output), Mat::zeros(image_output.size(),CV_32F) };
	split(image_transform, planes);//分离通道，获取实部虚部
	Mat image_transform_real = planes[0];
	Mat image_transform_imag = planes[1];

	int core_x = image_transform_real.rows / 2;//频谱图中心坐标
	int core_y = image_transform_real.cols / 2;
	int r = 60;  //滤波半径
	float h;
	for (int i = 0; i < image_transform_real.rows; i++)
	{
		for (int j = 0; j < image_transform_real.cols; j++)
		{
			h = exp(-((i - core_x) * (i - core_x) + (j - core_y) * (j - core_y)) / (2 * r * r));
			image_transform_real.at<float>(i, j) = image_transform_real.at<float>(i, j) * h;
			image_transform_imag.at<float>(i, j) = image_transform_imag.at<float>(i, j) * h;
		}
	}
	planes[0] = image_transform_real;
	planes[1] = image_transform_imag;
	Mat image_transform_ilpf;//定义高斯低通滤波结果
	merge(planes, 2, image_transform_ilpf);

	//3、傅里叶逆变换
	Mat iDft[] = { Mat_<float>(image_output), Mat::zeros(image_output.size(),CV_32F) };
	idft(image_transform_ilpf, image_transform_ilpf);//傅立叶逆变换
	split(image_transform_ilpf, iDft);//分离通道，主要获取0通道
	magnitude(iDft[0], iDft[1], iDft[0]); //计算复数的幅值，保存在iDft[0]
	normalize(iDft[0], iDft[0], 0, 1, NORM_MINMAX);//归一化处理
	//imshow("idft", iDft[0]);//显示逆变换图像
	dst = iDft[0].clone();
}

void MLImageDetection::ImageFilter::dftML(cv::Mat input_image, cv::Mat& output_image, cv::Mat& transform_image)
{

	//1.扩展图像矩阵，为2，3，5的倍数时运算速度快
	int m = getOptimalDFTSize(input_image.rows);
	int n = getOptimalDFTSize(input_image.cols);
	copyMakeBorder(input_image, input_image, 0, m - input_image.rows, 0, n - input_image.cols, BORDER_CONSTANT, Scalar::all(0));

	//2.创建一个双通道矩阵planes，用来储存复数的实部与虚部
	Mat planes[] = { Mat_<float>(input_image), Mat::zeros(input_image.size(), CV_32F) };

	//3.从多个单通道数组中创建一个多通道数组:transform_image。函数Merge将几个数组合并为一个多通道阵列，即输出数组的每个元素将是输入数组元素的级联
	merge(planes, 2, transform_image);

	//4.进行傅立叶变换
	dft(transform_image, transform_image);

	//5.计算复数的幅值，保存在output_image（频谱图）
	split(transform_image, planes); // 将双通道分为两个单通道，一个表示实部，一个表示虚部
	Mat transform_image_real = planes[0];
	Mat transform_image_imag = planes[1];

	magnitude(planes[0], planes[1], output_image); //计算复数的幅值，保存在output_image（频谱图）

	//6.前面得到的频谱图数级过大，不好显示，因此转换
	output_image += Scalar(1);   // 取对数前将所有的像素都加1，防止log0
	log(output_image, output_image);   // 取对数
	normalize(output_image, output_image, 0, 1, NORM_MINMAX); //归一化

	//7.剪切和重分布幅度图像限
	output_image = output_image(Rect(0, 0, output_image.cols & -2, output_image.rows & -2));

	// 重新排列傅里叶图像中的象限，使原点位于图像中心
	int cx = output_image.cols / 2;
	int cy = output_image.rows / 2;
	Mat q0(output_image, Rect(0, 0, cx, cy));   // 左上区域
	Mat q1(output_image, Rect(cx, 0, cx, cy));  // 右上区域
	Mat q2(output_image, Rect(0, cy, cx, cy));  // 左下区域
	Mat q3(output_image, Rect(cx, cy, cx, cy)); // 右下区域

	  //交换象限中心化
	Mat tmp;
	q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);//左上与右下进行交换
	q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);//右上与左下进行交换


	Mat q00(transform_image_real, Rect(0, 0, cx, cy));   // 左上区域
	Mat q01(transform_image_real, Rect(cx, 0, cx, cy));  // 右上区域
	Mat q02(transform_image_real, Rect(0, cy, cx, cy));  // 左下区域
	Mat q03(transform_image_real, Rect(cx, cy, cx, cy)); // 右下区域
	q00.copyTo(tmp); q03.copyTo(q00); tmp.copyTo(q03);//左上与右下进行交换
	q01.copyTo(tmp); q02.copyTo(q01); tmp.copyTo(q02);//右上与左下进行交换

	Mat q10(transform_image_imag, Rect(0, 0, cx, cy));   // 左上区域
	Mat q11(transform_image_imag, Rect(cx, 0, cx, cy));  // 右上区域
	Mat q12(transform_image_imag, Rect(0, cy, cx, cy));  // 左下区域
	Mat q13(transform_image_imag, Rect(cx, cy, cx, cy)); // 右下区域
	q10.copyTo(tmp); q13.copyTo(q10); tmp.copyTo(q13);//左上与右下进行交换
	q11.copyTo(tmp); q12.copyTo(q11); tmp.copyTo(q12);//右上与左下进行交换

	planes[0] = transform_image_real;
	planes[1] = transform_image_imag;
	merge(planes, 2, transform_image);//将傅里叶变换结果中心化
}
