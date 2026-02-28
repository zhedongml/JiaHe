#include"ml_AF.h"
using namespace std;


algorithm::AutoFocus::AutoFocus()
{
}

algorithm::AutoFocus::~AutoFocus()
{
}
// Brenner
double algorithm::AutoFocus::brenner(cv::Mat &image)
{
    //	assert(image.empty());

    cv::Mat gray_img;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, CV_BGR2GRAY);
    }
    else
        gray_img = image.clone();
    double result = .0f;
    int m = 0;
    for (int i = 0; i < gray_img.rows; ++i)
    {
        uchar *data = gray_img.ptr<uchar>(i);
        for (int j = 0; j < gray_img.cols - 2; ++j)
        {
            result += pow(data[j + 2] - data[j], 2);
        }
    }
    return result / gray_img.total();
}
//Tenengrad
double algorithm::AutoFocus::tenengard(cv::Mat &image)
{
    // assert(image.empty());

    cv::Mat gray_img, sobel_x, sobel_y, G;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, CV_BGR2GRAY);
    }
    else
        gray_img = image.clone();
    //分别计算x/y方向梯度
    cv::Sobel(gray_img, sobel_x, CV_32FC1, 1, 0);
    cv::Sobel(gray_img, sobel_y, CV_32FC1, 0, 1);
    cv::multiply(sobel_x, sobel_x, sobel_x);
    cv::multiply(sobel_y, sobel_y, sobel_y);
    cv::Mat sqrt_mat = sobel_x + sobel_y;
    cv::sqrt(sqrt_mat, G);

    return cv::mean(sqrt_mat)[0];
}
//smd
double algorithm::AutoFocus::smd(cv::Mat &image)
{
    // assert(image.empty());

    cv::Mat gray_img, smd_image_x, smd_image_y, G;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, CV_BGR2GRAY);
    }
    else
        gray_img = image.clone();
    cv::Mat kernel_x(3, 3, CV_32F, cv::Scalar(0));
    kernel_x.at<float>(1, 2) = -1.0;
    kernel_x.at<float>(1, 1) = 1.0;
    cv::Mat kernel_y(3, 3, CV_32F, cv::Scalar(0));
    kernel_y.at<float>(0, 1) = -1.0;
    kernel_y.at<float>(1, 1) = 1.0;
    cv::filter2D(gray_img, smd_image_x, gray_img.depth(), kernel_x);
    cv::filter2D(gray_img, smd_image_y, gray_img.depth(), kernel_y);

    smd_image_x = cv::abs(smd_image_x);
    smd_image_y = cv::abs(smd_image_y);
    G = smd_image_x + smd_image_y;

    return cv::mean(G)[0];
}

double algorithm::AutoFocus::smd2(cv::Mat &image)
{
    // assert(image.empty());

    cv::Mat gray_img, smd_image_x, smd_image_y, G;
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, CV_BGR2GRAY);
    }
    else
        gray_img = image.clone();
    cv::Mat kernel_x(3, 3, CV_32F, cv::Scalar(0));
    kernel_x.at<float>(1, 2) = -1.0;
    kernel_x.at<float>(1, 1) = 1.0;
    cv::Mat kernel_y(3, 3, CV_32F, cv::Scalar(0));
    kernel_y.at<float>(1, 1) = 1.0;
    kernel_y.at<float>(2, 1) = -1.0;
    cv::filter2D(gray_img, smd_image_x, gray_img.depth(), kernel_x);
    cv::filter2D(gray_img, smd_image_y, gray_img.depth(), kernel_y);

    smd_image_x = cv::abs(smd_image_x);
    smd_image_y = cv::abs(smd_image_y);
    cv::multiply(smd_image_x, smd_image_y, G);

    return cv::mean(G)[0];
}
void comput_blur_IQA(cv::Mat &src, float &blur_mean, float &blur_ratio)
{
    cv::Mat gray_img = src.clone();
    //计算水平/竖直差值获取梯度图
    cv::Mat grad_h, grad_v;
    cv::Mat kernel_h = cv::Mat::zeros(cv::Size(3, 3), CV_32FC1);
    kernel_h.at<float>(0, 1) = -1;
    kernel_h.at<float>(2, 1) = 1;
    cv::filter2D(gray_img, grad_h, CV_32FC1, kernel_h);
    cv::Mat kernel_v = cv::Mat::zeros(cv::Size(3, 3), CV_32FC1);
    kernel_v.at<float>(1, 0) = -1;
    kernel_v.at<float>(1, 2) = 1;
    cv::filter2D(gray_img, grad_v, CV_32FC1, kernel_v);

    //获取候选边缘点
    //筛选条件：D_h > D_mean
    float mean = static_cast<float>(cv::mean(grad_v)[0]);
    cv::Mat mask = grad_h > mean;
    mask = mask / 255;
    mask.convertTo(mask, CV_32FC1);
    cv::Mat C_h;
    cv::multiply(grad_h, mask, C_h);

    //进一步筛选边缘点
    //筛选条件：C_h(x,y) > C_h(x,y-1) and C_h(x,y) > C_h(x,y+1)
    cv::Mat edge = cv::Mat::zeros(C_h.rows, C_h.cols, CV_8UC1);
    for (int i = 1; i < C_h.rows - 1; ++i)
    {
        float *prev = C_h.ptr<float>(i - 1);
        float *cur = C_h.ptr<float>(i);
        float *next = C_h.ptr<float>(i + 1);
        uchar *data = edge.ptr<uchar>(i);
        for (int j = 0; j < C_h.cols; ++j)
        {
            if (prev[j] < cur[j] && next[j] < cur[j])
            {
                data[j] = 1;
            }
        }
    }

    //检测边缘点是否模糊
    //获取inverse blur
    cv::Mat A_h = grad_h / 2;
    cv::Mat BR_h = cv::Mat(gray_img.size(), CV_32FC1);
    gray_img.convertTo(gray_img, CV_32FC1);
    cv::absdiff(gray_img, A_h, BR_h);
    cv::divide(BR_h, A_h, BR_h);
    cv::Mat A_v = grad_v / 2;
    cv::Mat BR_v;
    cv::absdiff(gray_img, A_v, BR_v);
    cv::divide(BR_v, A_v, BR_v);

    cv::Mat inv_blur = cv::Mat::zeros(BR_v.rows, BR_v.cols, CV_32FC1);
    for (int i = 0; i < inv_blur.rows; ++i)
    {
        float *data_v = BR_v.ptr<float>(i);
        float *data = inv_blur.ptr<float>(i);
        float *data_h = BR_h.ptr<float>(i);
        for (int j = 0; j < inv_blur.cols; ++j)
        {
            data[j] = data_v[j] > data_h[j] ? data_v[j] : data_h[j];
        }
    }
    //获取最终模糊点
    cv::Mat blur = inv_blur < 0.1 / 255;
    blur.convertTo(blur, CV_32FC1);

    //计算边缘模糊的均值和比例
    int sum_inv_blur = cv::countNonZero(inv_blur);
    int sum_blur = cv::countNonZero(blur);
    int sum_edge = cv::countNonZero(edge);
    blur_mean = static_cast<float>(sum_inv_blur) / sum_blur;
    blur_ratio = static_cast<float>(sum_blur) / sum_edge;
}
void compute_noise_IQA(cv::Mat &gray_img, float &noise_mean, float &noise_ratio)
{
    //均值滤波去除噪声对边缘检测的影响
    cv::Mat blur_img;
    cv::blur(gray_img, blur_img, cv::Size(3, 3));

    //进行竖直方向边缘检测
    cv::Mat grad_h, grad_v;
    cv::Mat kernel_h = cv::Mat::zeros(cv::Size(3, 3), CV_32FC1);
    kernel_h.at<float>(0, 1) = -1;
    kernel_h.at<float>(2, 1) = 1;
    cv::filter2D(gray_img, grad_h, CV_32FC1, kernel_h);
    cv::Mat kernel_v = cv::Mat::zeros(cv::Size(3, 3), CV_32FC1);
    kernel_v.at<float>(1, 0) = -1;
    kernel_v.at<float>(1, 2) = 1;
    cv::filter2D(gray_img, grad_v, CV_32FC1, kernel_v);

    //筛选候选点
    //水平/竖直梯度的均值
    float D_h_mean = .0f, D_v_mean = .0f;
    D_h_mean = static_cast<float>(cv::mean(grad_h)[0]);
    D_v_mean = static_cast<float>(cv::mean(grad_v)[0]);

    //获取候选噪声点
    cv::Mat N_cand = cv::Mat::zeros(gray_img.rows, gray_img.cols, CV_32FC1);
    for (int i = 0; i < gray_img.rows; ++i)
    {
        float *data_h = grad_h.ptr<float>(i);
        float *data_v = grad_v.ptr<float>(i);
        float *data = N_cand.ptr<float>(i);
        for (int j = 0; j < gray_img.cols; ++j)
        {
            if (data_v[j] < D_v_mean && data_h[j] < D_h_mean)
            {
                data[j] = data_v[j] > data_h[j] ? data_v[j] : data_h[j];
            }
        }
    }

    //最终的噪声点
    float N_cand_mean = static_cast<float>(cv::mean(N_cand)[0]);
    cv::Mat mask = (N_cand > N_cand_mean) / 255;
    mask.convertTo(mask, CV_32FC1);
    cv::Mat N;
    cv::multiply(N_cand, mask, N);

    //计算噪声的均值和比率
    float sum_noise = static_cast<float>(cv::sum(N)[0]);
    int sum_noise_cnt = cv::countNonZero(N);
    noise_mean = sum_noise / (sum_noise_cnt + 0.0001);
    noise_ratio = static_cast<float>(sum_noise_cnt) / N.total();
}
double algorithm::AutoFocus::blur_noise_IQA(cv::Mat &image)
{
    // assert(image.empty());

    cv::Mat gray_img = cv::Mat(image.size(), CV_8UC1);
    if (image.channels() == 3)
    {
        cv::cvtColor(image, gray_img, CV_BGR2GRAY);
    }
    else
        gray_img = image.clone();
    // 1、模糊检测
    float blur_mean = 0.f, blur_ratio = 0.f;
    comput_blur_IQA(gray_img, blur_mean, blur_ratio);

    // 2、噪声点检测
    float noise_mean = 0.f, noise_ratio = 0.f;
    compute_noise_IQA(gray_img, noise_mean, noise_ratio);

    // 3、噪声和模糊的组合
    double result = 1 - (blur_mean + 0.95 * blur_ratio + 0.3 * noise_mean + 0.75 * noise_ratio);
    return abs(result);
}