#include "ImageSwitch.h"

ImageSwitch::ImageSwitch()
{
}

ImageSwitch::~ImageSwitch()
{
}

Result ImageSwitch::xyz2rgb(QImage &rgbImg, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage)
{
    cv::Mat rgbMat;
    Result ret = xyz2rgbMat(rgbMat, xImage, yImage, zImage);
    if(!ret.success){
        return ret;
    }

    cvMat2QImage(rgbMat, rgbImg);
    return Result();
}

Result ImageSwitch::xyz2rgbMat(cv::Mat &rgbMat, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage)
{
    if (xImage.empty() || yImage.empty() || zImage.empty())
    {
        return Result(false, "XYZ image exist empty.");
    }

    if (xImage.size() != yImage.size() || xImage.size() != zImage.size())
    {
        return Result(false, "XYZ image size is different.");
    }

    return xyz2rgbDiy(rgbMat, xImage, yImage, zImage);
}

Result ImageSwitch::xyz2rgbUtil(cv::Mat &rgbMat, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage)
{
    cv::Mat X, Y, Z;
    xImage.convertTo(X, CV_32FC1);
    yImage.convertTo(Y, CV_32FC1);
    zImage.convertTo(Z, CV_32FC1);

    {
        cv::Mat R = getRMatrix();
        cv::Mat Xn = X * R.at<float>(0, 0) + Y * R.at<float>(0, 1) + Z * R.at<float>(0, 2);
        cv::Mat Yn = X * R.at<float>(1, 0) + Y * R.at<float>(1, 1) + Z * R.at<float>(1, 2);
        cv::Mat Zn = X * R.at<float>(2, 0) + Y * R.at<float>(2, 1) + Z * R.at<float>(2, 2);
        X = Xn;
        Y = Yn;
        Z = Zn;
    }

    cv::Mat R = cv::Mat::zeros(X.size(), X.type());
    cv::Mat G = cv::Mat::zeros(X.size(), X.type());
    cv::Mat B = cv::Mat::zeros(X.size(), X.type());

    for (int i = 0; i < X.rows; i++)
    {
        for (int j = 0; j < X.cols; j++)
        {
            float x = X.at<float>(i, j);
            float y = Y.at<float>(i, j);
            float z = Z.at<float>(i, j);

            const colorutil::XYZ xyz_color(x, y, z);
            const colorutil::RGB rgb_color = colorutil::convert_XYZ_to_RGB(xyz_color);
            R.at<float>(i, j) = rgb_color(0);
            G.at<float>(i, j) = rgb_color(1);
            B.at<float>(i, j) = rgb_color(2);
        }
    }
    //G.setTo(0, G > 65000);
    vector<cv::Mat>imgV;
    R.convertTo(R, CV_8UC1);
    G.convertTo(G, CV_8UC1);
    B.convertTo(B, CV_8UC1);
    imgV.push_back(B);
    imgV.push_back(G);
    imgV.push_back(R);

    cv::merge(imgV, rgbMat);
    return Result();
}

Result ImageSwitch::xyz2rgbDiy(cv::Mat &rgbMat, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage)
{
    cv::Mat X, Y, Z;
    xImage.convertTo(X, CV_32FC1);
    yImage.convertTo(Y, CV_32FC1);
    zImage.convertTo(Z, CV_32FC1);

    //{
    //    cv::Mat R = getRMatrix();
    //    cv::Mat Xn = X * R.at<float>(0, 0) + Y * R.at<float>(0, 1) + Z * R.at<float>(0, 2);
    //    cv::Mat Yn = X * R.at<float>(1, 0) + Y * R.at<float>(1, 1) + Z * R.at<float>(1, 2);
    //    cv::Mat Zn = X * R.at<float>(2, 0) + Y * R.at<float>(2, 1) + Z * R.at<float>(2, 2);
    //    X = Xn;
    //    Y = Yn;
    //    Z = Zn;
    //}

    cv::Mat R = cv::Mat::zeros(X.size(), X.type());
    cv::Mat G = cv::Mat::zeros(X.size(), X.type());
    cv::Mat B = cv::Mat::zeros(X.size(), X.type());

    for (int i = 0; i < X.rows; i++)
    {
        for (int j = 0; j < X.cols; j++)
        {
            // X, Y and Z input refer to a D65/2бу standard illuminant.
            // sR, sG and sB (standard RGB) output range = 0 б┬ 255
            float var_X = X.at<float>(i, j);
            float var_Y = Y.at<float>(i, j);
            float var_Z = Z.at<float>(i, j);

            {
                cv::Mat R = getRMatrix();
                float Xn = var_X * R.at<float>(0, 0) + var_Y * R.at<float>(0, 1) + var_Z * R.at<float>(0, 2);
                float Yn = var_X * R.at<float>(1, 0) + var_Y * R.at<float>(1, 1) + var_Z * R.at<float>(1, 2);
                float Zn = var_X * R.at<float>(2, 0) + var_Y * R.at<float>(2, 1) + var_Z * R.at<float>(2, 2);
                var_X = Xn;
                var_Y = Yn;
                var_Z = Zn;
            }

            float var_R = 3.2404542 * var_X + (-1.5371385) * var_Y + (-0.4985314) * var_Z;
            float var_G = (-0.9692660) * var_X + (1.8760108) * var_Y + (0.0415560) * var_Z;
            float var_B = (0.0556434) * var_X + (-0.2040259) * var_Y + (1.0572252) * var_Z;

           if (var_R > 0.0031308) 
                //var_R = pow(1.055 * var_R, (1 / 2.4)) - 0.055;
               //var_R = 1.055 * pow(var_R, (1 / 2.4)) - 0.055;
                var_R = pow((var_R - 0.055) * 1.055, 1 / 2.4);
            else 
                var_R = 12.92 * var_R;
            if (var_G > 0.0031308)
                //var_G = pow(1.055 * var_G, (1 / 2.4)) - 0.055;
                //var_G = 1.055 * pow(var_G, (1 / 2.4)) - 0.055;
                var_G = pow((var_G - 0.055) * 1.055, 1 / 2.4);
            else
                var_G = 12.92 * var_G;
            if (var_B > 0.0031308)
                //var_B = pow(1.055 * var_B, (1 / 2.4)) - 0.055;
                //var_B = 1.055 * pow(var_B, (1 / 2.4)) - 0.055;
                var_B = pow((var_B - 0.055) * 1.055, 1 / 2.4);
            else
                var_B = 12.92 * var_B;

            R.at<float>(i, j) = var_R;
            G.at<float>(i, j) = var_G;
            B.at<float>(i, j) = var_B;
        }
    }

    R.convertTo(R, CV_8UC1);
    G.convertTo(G, CV_8UC1);
    B.convertTo(B, CV_8UC1);

    vector<cv::Mat> imgV;
    imgV.push_back(B);
    imgV.push_back(G);
    imgV.push_back(R);
    cv::merge(imgV, rgbMat);
    return Result();
}

Result ImageSwitch::xyz2rgbCvt(cv::Mat &rgbMat, const cv::Mat &xImage, const cv::Mat &yImage, const cv::Mat &zImage)
{
    //cvtColor(src, yuv, COLOR_XYZ2RGB);

    //cvtColor();
    return Result();
}

bool ImageSwitch::cvMat2QImage(const cv::Mat &mat, QImage &image)
{
    switch (mat.type())
    {
    case CV_8UC1:
        image = QImage((const unsigned char *)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        break;
    case CV_8UC3:
        image = QImage((const unsigned char *)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        image = image.rgbSwapped();
        break;
    case CV_8UC4:
        image = QImage((const unsigned char *)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        break;
    case CV_16UC4:
        image = QImage((const unsigned char *)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
        image = image.rgbSwapped();
        break;
    }
    return true;
}

cv::Mat ImageSwitch::getRMatrix()
{
    int rows = 3;
    int cols = 3;
    cv::Mat m = cv::Mat(rows, cols, CV_32F);

    //m.at<float>(0, 0) = 0.810426796;
    //m.at<float>(0, 1) = 0.20759108;
    //m.at<float>(0, 2) = 0.11400796;

    //m.at<float>(1, 0) = -0.043153918;
    //m.at<float>(1, 1) = 0.826252064;
    //m.at<float>(1, 2) = 0.029919817;

    //m.at<float>(2, 0) = -0.00658873;
    //m.at<float>(2, 1) = -0.003127408;
    //m.at<float>(2, 2) = 2.032100277;

    m.at<float>(0, 0) = 6.393652439117432;
    m.at<float>(0, 1) = 0.7989959120750427;
    m.at<float>(0, 2) = 3.2075936794281006;

    m.at<float>(1, 0) = 1.1064904928207397;
    m.at<float>(1, 1) = 6.095374584197998;
    m.at<float>(1, 2) = -1.3828397989273071;

    m.at<float>(2, 0) = 0.08643676340579987;
    m.at<float>(2, 1) = -0.38640448451042175;
    m.at<float>(2, 2) = 28.209915161132813;

    return m;
}


