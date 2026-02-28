#include "ml_distortionPara.h"
#include <numeric>
using namespace algorithm;

algorithm::MLDistortionPara::MLDistortionPara()
{
}

algorithm::MLDistortionPara::~MLDistortionPara()
{
}

void algorithm::MLDistortionPara::SetGridImage(cv::Mat img_grid)
{
    m_img_grid = img_grid;
    hydrusConfig.GetNewPara(img_grid);
    m_para = hydrusConfig.m_para;
}
void algorithm::MLDistortionPara::SetFovAngle(double fovH, double fovV)
{
    m_fovH = fovH;
    m_fovV = fovV;
}
void algorithm::MLDistortionPara::SetRotationDeg(float degree)
{
    m_rotationAngle = degree;
}
DistortionRe algorithm::MLDistortionPara::GetGridDistortion()
{
    DistortionRe re;
    re = GridDistortion(m_img_grid);
    return re;
}
std::vector<double> algorithm::MLDistortionPara::GetDistortionCorner()
{
    return m_distortionRe.disCorner;
}
double algorithm::MLDistortionPara::GetDistortionAverage()
{
    if (m_distortionRe.disAvg != -1)
    {
        return m_distortionRe.disAvg;
    }
    else
    {
        GridDistortion(m_img_grid);
        return m_distortionRe.disAvg;
    }
}
cv::Mat algorithm::MLDistortionPara::GetCornerPositionsX()
{
    if (m_distortionRe.xPos.data != NULL)
    {
        return m_distortionRe.xPos;
    }
    else
    {
        GridDistortion(m_img_grid);
        return m_distortionRe.xPos;
    }
}
cv::Mat algorithm::MLDistortionPara::GetCornerPositionsY()
{
    if (m_distortionRe.yPos.data != NULL)
    {
        return m_distortionRe.yPos;
    }
    else
    {
        GridDistortion(m_img_grid);
        return m_distortionRe.yPos;
    }
}
double algorithm::MLDistortionPara::GetNewfocalLength()
{
    if (m_distortionRe.newFocalLength > 0)
    {
        return m_distortionRe.newFocalLength;
    }
    else
    {
        GridDistortion(m_img_grid);
        return m_distortionRe.newFocalLength;
    }
}
cv::Mat algorithm::MLDistortionPara::getImgDraw()
{
    return m_img_draw;
}
DistortionRe algorithm::MLDistortionPara::GridDistortion(cv::Mat img, int flag, cv::Rect rect)
{
    DistortionRe re;
    if (img.data != NULL)
    {
        m_img_draw.release();
         double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        img = getRotationImg(img, m_rotationAngle);
        cv::Mat roi;
        cv::Rect rectangle = m_para.rect; // GetNewPara(img).rect;
        double m_FocalLength = HydrusParameters::FocalLength;
        double m_pixel_size = m_para.pixel_size;
        if (rectangle.x != 0 || rectangle.y != 0)
            roi = img(rectangle);
        else
            roi = img.clone();
        cv::Mat roi1 = convertToUint8(roi);

        // TODO: add test
        // cv::Rect rectNew;
        // rectNew = m_fov.roiRect;
        cv::Rect rectGrid = getGridRect(roi1);
        // TODO::
        cv::Mat roi2 = roi1.clone(); // GetROIMat(roi1, rectGrid);
        rectGrid.x = 0;
        rectGrid.y = 0;
        if (rectGrid.width < 0)
        {
            rectGrid.x = 0;
            rectGrid.y = 0;
        }

        //cv::Point2f center((float)(roi2.cols / 2), (float)(roi2.rows / 2));
        //cv::Mat affine_matrix = getRotationMatrix2D(center, m_rotationDeg, 1.0);
        //warpAffine(roi2, roi2, affine_matrix, roi2.size());
        cv::Mat img_draw = convertToUint8(roi2);

        // test end
        vector<Point2f> corns;
        cv::Mat ordList;
        int m_gridAlgorthm = HydrusParameters::gridAlgorthm;
        MLGridDetect grid;
        grid.SetbinNum(m_para.binNum);
        if (m_gridAlgorthm == 1)
        {
            // corns = grid.GridHoughLine(roi2, ordList);
            corns = grid.GridContour(roi2, ordList);
        }
        else if (m_gridAlgorthm == 2)
        {
            corns = grid.GridCorners(roi2, ordList);
        }
        else if (m_gridAlgorthm == 3)
        {
            corns = grid.GridTemplateGaussian(roi2, ordList);
        }
        m_img_draw = roi2.clone();
        if ((corns.size() == (ordList.rows * ordList.cols)) & (ordList.data != NULL))
        {
            cv::Point2f center;
            vector<cv::Point2f> corners;
            std::vector<double> disCorner;
            center = getCenter(corns, ordList);
            //center = center + Point2f(rectangle.x, rectangle.y) + Point2f(rectGrid.x, rectGrid.y);
            circle(img_draw, center, 10, cv::Scalar(255, 0, 255), -1);
            cv::Point2f BL = corns[ordList.at<short>(ordList.cols - 1, 0)];
            corners.push_back(BL);
            cv::Point2f BR = corns[ordList.at<short>(ordList.cols - 1, ordList.rows - 1)];
            corners.push_back(BR);
            cv::Point2f UL = corns[ordList.at<short>(0, 0)];
            corners.push_back(UL);
            cv::Point2f UR = corns[ordList.at<short>(0, ordList.rows - 1)];
            corners.push_back(UR);
            for (int i = 0; i < corners.size();i++)
            {
                //Point2f temp = corners[i] + Point2f(rectangle.x, rectangle.y) + Point2f(rectGrid.x, rectGrid.y);
                //Newcorners.push_back(corners[i]);
                circle(img_draw, corners[i], 10, cv::Scalar(255, 0, 255), -1);
                double val = getCornerDistortionVal(corners[i], center);
                disCorner.push_back(val);
            }
            double disAvg = -1;
            disAvg = accumulate(disCorner.begin(), disCorner.end(), 0.0) / disCorner.size();
            disCorner.push_back(disAvg);
            cv::Mat xPos(ordList.rows, ordList.cols, CV_32FC1);
            cv::Mat yPos(ordList.rows, ordList.cols, CV_32FC1);
            int m = 0;
            cv::Mat xPosDis(ordList.rows, ordList.cols, CV_32FC1);
            cv::Mat yPosDis(ordList.rows, ordList.cols, CV_32FC1);


            for (int i = 0; i < ordList.rows; i++)
            {
                for (int j = 0; j < ordList.cols; j++)
                {
                    // TODO:
                    int in = ordList.at<short>(i, j);
                    double x = corns[in].x /*+ rectangle.x + rectGrid.x*/;
                    double y = corns[in].y /*+ rectangle.y + rectGrid.y*/;
                    // double x = corns[i * 11 + j].x + rectangle.x + rectGrid.x;
                    // double y = corns[i * 11 + j].y + rectangle.y + rectGrid.y;

                   // xPosDis.at<float>(i, j) = x;
                   // yPosDis.at<float>(i, j) = y;

                    double deltx = abs(x - center.x);
                    double delty = abs(y - center.y);
                    xPosDis.at<float>(i, j) = deltx;
                    yPosDis.at<float>(i, j) = delty;
                    xPos.at<float>(i, j) = atan(deltx * m_pixel_size / m_FocalLength) * 180 / CV_PI;
                    yPos.at<float>(i, j) = atan(delty * m_pixel_size / m_FocalLength) * 180 / CV_PI;
                    cv::Point2f pt = cv::Point2f(x, y);
                    circle(img_draw, pt, 5, cv::Scalar(255, 0, 255), -1);
                }
            }
            m_img_draw = img_draw.clone();
            re.disCorner.clear();
            re.disCorner=disCorner;
            re.disAvg = disAvg;
            re.xPos = xPos;
            re.yPos = yPos;
            m_distortionRe = re;
        }
    }
    return m_distortionRe;
}
cv::Point2f algorithm::MLDistortionPara::getCenter(vector<Point2f> corns, cv::Mat indexMap)
{
    cv::Point2f center;
    int col = indexMap.cols;
    int row = indexMap.rows;
    if ((indexMap.rows / 2) % 2 == 0 && (indexMap.cols / 2) % 2 == 0)
    {
        cv::Point2f cor1 = corns[indexMap.at<short>(row / 2 - 1,col / 2 - 1)];
        cv::Point2f cor2 = corns[indexMap.at<short>(row / 2 - 1, col / 2)];
        cv::Point2f cor3 = corns[indexMap.at<short>(row / 2, col / 2 - 1)];
        cv::Point2f cor4 = corns[indexMap.at<short>(row / 2, col / 2)];
        center = (cor1 + cor2 + cor3 + cor4) / 4.0;
        circle(m_img_draw, center, 5, cv::Scalar(255, 0, 255), -1);
    }
    else if ((indexMap.rows / 2) % 2 == 1 && (indexMap.cols / 2) % 2 == 1)
    {
        center = corns[indexMap.at<short>((row - 1) / 2, (col - 1) / 2)];
        circle(m_img_draw, center, 5, cv::Scalar(255, 0, 255), -1);
    }
    else if ((indexMap.rows / 2) % 2 == 0 && (indexMap.cols / 2) % 2 == 1)
    {
        cv::Point2f cor1 = corns[indexMap.at<short>(row / 2 - 1,(col - 1) / 2)];
        cv::Point2f cor2 = corns[indexMap.at<short>(row / 2,(col - 1) / 2)];
        center = (cor1 + cor2) / 2.0;
        circle(m_img_draw, center, 5, cv::Scalar(255, 0, 255), -1);
    }
    else if ((indexMap.rows / 2) % 2 == 1 && (indexMap.cols / 2) % 2 == 0)
    {
        cv::Point2f cor1 = corns[indexMap.at<short>((row-1) / 2, col / 2-1)];
        cv::Point2f cor2 = corns[indexMap.at<short>((row - 1) / 2, col / 2)];
        center = (cor1 + cor2) / 2.0;
        circle(m_img_draw, center, 5, cv::Scalar(255, 0, 255), -1);
    }
    return center;
}
double algorithm::MLDistortionPara::getCornerDistortionVal(cv::Point2f corner, cv::Point2f center)
{
    double val = -1;
    double fovH, fovV;
    double m_FocalLength = HydrusParameters::FocalLength;
    double m_pixel_size = m_para.pixel_size;
    if (m_fovH != -1 && m_fovV != -1)
    {
        fovH = m_fovH;
        fovV = m_fovV;
    }
    else
    {
        double deg = 14.0;
        fovH = deg;
        fovV = deg;
    }
    double d = Getdistance(corner, center);
    double medVar = m_FocalLength * tan(sqrt(pow((fovH / 2), 2) + pow((fovV / 2), 2)) / 180.0 * CV_PI);
    d = d * m_pixel_size;
    // cout << d << "," << medVar << endl;
    val = 100 * /*abs*/((d - medVar) / medVar);
    return val;
}
cv::Rect algorithm::MLDistortionPara::getGridRect(cv::Mat roi)
{
    cv::Rect rect;
    if (roi.data != NULL)
    {
        cv::Mat img_gray, img_gray1, img_draw;
        if (roi.depth() == CV_8U)
        {
            img_gray1 = roi.clone();
            img_draw = roi.clone();
        }
        else
        {
            MLimagePublic mlp;
            img_gray1 = mlp.convertToUint8(roi);
            img_draw = mlp.convertToUint8(roi);
        }
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::Mat img_process, srcbinary;
        cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        // cv::GaussianBlur(img_gray1, img_gray1, cv::Size(9, 9), 2, 2);
        // cv::morphologyEx(img_gray1, img_gray1, cv::MORPH_CLOSE, element1);
        // cv::morphologyEx(img_gray1, img_process, cv::MORPH_GRADIENT, element1);
        threshold(img_gray1, srcbinary, 0, 255, THRESH_TRIANGLE);
        // cv::Mat img_canny;
        // cv::Canny(srcbinary, img_canny, 50, 150);
        cv::findContours(srcbinary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
        vector<double> area_sort;
        vector<int> list;
        cv::Mat roi;
        RotatedRect rec;
        for (int i = 0; i < contours.size(); ++i)
        {
            double area = contourArea(contours[i], false);
            // cout << area << endl;
            double powbin = 2; // pow(m_para.binNum, 2);
            if (area > 1e6)
            // if (area > 4e6 / powbin && area < 6e6 / powbin)
            {
                rect = cv::boundingRect(cv::Mat(contours[i]));
                cv::rectangle(img_draw, rect, Scalar(255, 0, 255), 1);
            }
        }
    }
    return rect;
}
