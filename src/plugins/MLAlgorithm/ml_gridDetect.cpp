#include "ml_gridDetect.h"
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include<numeric> // accumulate定义的头文件
#include "ml_chessboardDetect.h"
#include<time.h>
#include"CrossCenter.h"

algorithm::MLGridDetect::MLGridDetect()
{
}

algorithm::MLGridDetect::~MLGridDetect()
{
}

std::vector<cv::Point2f> algorithm::MLGridDetect::GridCorners(cv::Mat img, cv::Mat &indexMap)
{
    vector<Point2f> ptsNew;
    if (img.data != NULL)
    {
        cv::Mat img_draw = convertTo3Channels(img);
        Mat srcgray = convertToGrayImage(img);
       
        cv::GaussianBlur(srcgray, srcgray, cv::Size(3, 3), 0, 0);
        Mat srcbinary;
        threshold(srcgray, srcbinary, 0, 255, THRESH_OTSU);
        // 2、Shi-Tomasi
        vector<Point2f> corners; 
        int maxcorners = 2000;
        double qualityLevel = 0.01;
        double minDistance = 20;
        int blockSize = 20;
        double k = 0.06; 
        goodFeaturesToTrack(srcgray, corners, maxcorners, qualityLevel, minDistance, Mat(), blockSize, false, k);
        Size winSize = Size(5, 5);    
        Size zeroZone = Size(-1, -1); 
        TermCriteria criteria = TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001);
        // TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 40, 0.001);
     //   cv::cornerSubPix(srcgray, corners, winSize, zeroZone, criteria);

        drawPointsOnImage(img_draw, corners, 2, Scalar(0, 0, 255));

        vector<Point2f> cornersNew;
        cornersNew = pointsClusters(corners, 50/m_binNum);
        drawPointsOnImage(img_draw, cornersNew, 2, Scalar(0, 255, 0));

        // point filter, it's hard to find the right point
        vector<Point2f> cornersNew1;
        for (int i = 0; i < cornersNew.size(); i++)
        {
          //  cout << cornersNew[i] << endl;
            double left = cv::max(0, int(cornersNew[i].x - 10));
            double right = cv::min(srcgray.rows, int(cornersNew[i].x + 10));
            double top = cv::max(0, int(cornersNew[i].y - 10));
            double bottom = cv::min(srcgray.cols, int(cornersNew[i].y + 10));
            cv::Mat roi = srcgray(cv::Rect(left, top, right - left, bottom - top));
            double corr = corrlateMat90(roi);
            double mag = getMagnitude(roi);
             //cout << corr << " ," << mag << endl;
            if (mag>500)
            {
                cornersNew1.push_back(cornersNew[i]);
                circle(img_draw, cornersNew[i], 5, Scalar(255, 0, 255), -1, 8, 0);
            }
        }
        vector<Point2f>pts = cornersNew1;
        if (pts.size() > 0)
        {
            // dian fen lei   
            ptsNew = pointsClusters(pts, 50/m_binNum);
            cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, false, 60/m_binNum);
            MLOptParaChessboard cb;
            indexMap = cb.updateIndexMapKL(indexMap1, ptsNew, corners);
            //corners = ptsNew;
            //draw
            for (int i = 0; i < indexMap.rows; i++)
            {
                for (int j = 0; j < indexMap.cols; j++)
                {
                    int in = indexMap.at<short>(i, j);
                    if (in < corners.size())
                    {
                        circle(img_draw, corners[in], 2, Scalar(255, 0, 255), -1);
                        // cout << corners[in] << endl;
                    }
                }
                //  cout << endl;
            }
        }
        m_img_draw = img_draw.clone();

    }
    return ptsNew;
}

vector<cv::Point2f> algorithm::MLGridDetect::GridHoughLine(cv::Mat roi, cv::Mat &ordNew, cv::Size boadsize)
{
        vector<cv::Point2f> corners;
        // cv::Mat img_draw = roi.clone();
        // roi.convertTo(roi, CV_8UC1);
        cv::Mat srcgray;
        cv::Mat srcbinary;
        cv::Mat img_gray, img_draw;
        if (roi.channels() == 3)
            cvtColor(roi, img_gray, cv::COLOR_BGR2GRAY);
        else
            img_gray = roi.clone();
        if (img_gray.depth() == CV_8U)
        {
            srcgray = img_gray.clone();
            img_draw = roi.clone();
        }
        else
        {
            srcgray = convertToUint8(img_gray);
            img_draw = convertToUint8(roi);
        }
        cv::threshold(srcgray, srcbinary, 0, 255, CV_THRESH_OTSU);
        // cv::threshold(srcgray, srcbinary, 0, 255, CV_THRESH_TRIANGLE);

        int row = srcgray.rows;
        int col = srcgray.cols;
        double minv = 0.0, maxv = 0.0;
        Point minp, maxp;
        cv::minMaxLoc(srcgray, &minv, &maxv, &minp, &maxp);
        Mat m;
        srcgray.convertTo(m, CV_64FC1);
        Mat m_row(1, col, CV_64FC1, Scalar(0));
        Mat m_col(row, 1, CV_64FC1, Scalar(0));
        reduce(m, m_row, 0, CV_REDUCE_SUM);
        reduce(m, m_col, 1, CV_REDUCE_SUM);
        int numR = countNonZero(m_row);
        int numC = countNonZero(m_col);
        int l = (numR < numC ? numR : numC) / 18;
        cv::Mat srccanny;
        cv::Canny(srcbinary, srccanny, 30, 90);
        vector<Vec4i> lines;
        vector<Vec4i> lines1, lines2;
        vector<double> ks1, ks2;
        vector<double> b1, b2;
        cv::HoughLinesP(srccanny, lines, 1, CV_PI / 180, l, l, l);
        for (int i = 0; i < lines.size(); i++)
        {
            cv::Point point0;
            point0.x = lines[i][0];
            point0.y = lines[i][1];
            double k = double((lines[i][3] - lines[i][1])) / double((lines[i][2] - lines[i][0]));
            double b = point0.y - k * point0.x;
            if (abs(k) < 1)
            {
                ks1.push_back(k);
                b1.push_back(b);
                lines1.push_back(lines[i]);
                line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 0, 255), 1);
            }
            else if (abs(k) > 5)
            {
                k = double((lines[i][2] - lines[i][0])) / double((lines[i][3] - lines[i][1]));
                b = -(k * point0.y - point0.x);
                ks2.push_back(k);
                b2.push_back(b);
                lines2.push_back(lines[i]);
                //line(img_draw, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(255, 255, 0), 1);
            }
        }

        Mat b10(lines1.size(), 1, CV_32F);
        Mat b20(lines2.size(), 1, CV_32F);
        // ks1 分类
        for (int i = 0; i < b1.size(); i++)
        {
            b10.at<float>(i, 0) = b1[i];
        }
        Mat labelB1;
        Mat centersB1;
        TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 0.1);
        cv::kmeans(b10, boadsize.width, labelB1, criteria, 1, KMEANS_PP_CENTERS, centersB1);

        Scalar colorTab[] = {//定义一个颜色的数组
                             Scalar(0, 0, 255),   Scalar(0, 255, 0),    Scalar(255, 0, 0),   Scalar(0, 255, 255),
                             Scalar(100, 0, 255), Scalar(50, 100, 255), Scalar(150, 0, 255), Scalar(200, 50, 255),
                             Scalar(220, 0, 200), Scalar(120, 0, 100),  Scalar(255, 0, 120)};
        vector<vector<Vec4i>> linesH;
        for (int i = 0; i < labelB1.rows; i++)
        {
            int index = labelB1.at<int>(i);
            //line(img_draw, Point(lines1[i][0], lines1[i][1]), Point(lines1[i][2], lines1[i][3]), colorTab[index], 1);
        }
        double k10 = 0;
        double sum = accumulate(ks1.begin(), ks1.end(), 0.0);
        k10 = sum / ks1.size();
        cv::Mat b1Ord;
        cv::sort(centersB1, b1Ord, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

        // ks2 fenlei
        for (int i = 0; i < b2.size(); i++)
        {
            b20.at<float>(i, 0) = b2[i];
        }
        Mat labelB2;
        Mat centersB2;
        cv::kmeans(b20, boadsize.height, labelB2, criteria, 1, KMEANS_PP_CENTERS, centersB2);
        double k20 = 0;
        double sum2 = accumulate(ks2.begin(), ks2.end(), 0.0);
        k20 = sum2 / ks2.size();
        cv::Mat b2Ord, b2OrdIdx;
        cv::sort(centersB2, b2Ord, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
        cv::sortIdx(centersB2, b2OrdIdx, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

        // draw lines1
        for (int i = 0; i < centersB1.rows; i++)
        {
            double k = k10;
            double b = centersB1.at<float>(i);
            cv::Point point1, point2;
            point1.x = 0;
            point1.y = k * 0 + b;
            point2.x = 3000;
            point2.y = k * 3000 + b;
            cv::line(img_draw, point1, point2, cv::Scalar(255, 255, 0), 1, 8, 0);
        }
        // draw lines2
        for (int i = 0; i < centersB2.rows; i++)
        {
            double k = k20;
            double b = centersB2.at<float>(i);
            cv::Point point1, point2;
            point1.x = 1;
            point1.y = k * point1.x + b;
            point2.x = 3000;
            point2.y = k * point2.x + b;
            cv::Point point10, point20;
            point10.x = point1.y;
            point10.y = point1.x;
            point20.x = point2.y;
            point20.y = point2.x;

            cv::line(img_draw, point10, point20, cv::Scalar(255, 0, 255), 1, 8, 0);
        }
        // calculate crosssection
        ordNew = Mat::zeros(boadsize.width, boadsize.height, CV_16UC1);
        for (int i = 0; i < b1Ord.rows; i++)
        {
            for (int j = 0; j < b2Ord.rows; j++)
            {
                double b1 = b1Ord.at<float>(i);
                double k1 = k10;
                double k21 = k20;
                double b21 = b2Ord.at<float>(j);
                double k2, b2;
                if (abs(k21) ==0)
                {
                    k21 = 1e-6;
                }
                 k2 = 1.0 / k21; // ;cv::min(k21, 1e6);
                 b2 = -b21 /k21;// cv::min(k21, 1e6);
                cv::Point2f point;
                double x = 0, y = 0;
                x = (b2 - b1) / (k1 - k2);
                y = k1 * x + b1;
                point.x = x;
                point.y = y;
                corners.push_back(point);
                ordNew.at<short>(i, j) = (corners.size() - 1);
                //	draw
                circle(img_draw, point, 5, cv::Scalar(0, 0, 255), -1);
                if (i==5&j==5)
                {
                    circle(img_draw, point, 10, cv::Scalar(255, 0, 255), -1);
                }
            }
            // cout << endl;
        }
    m_img_draw = img_draw.clone();
        return corners;   
}

vector<cv::Point2f> algorithm::MLGridDetect::GridContour(cv::Mat img, cv::Mat &indexMap)
{
    vector<cv::Point2f> corners;
    if (img.data != NULL)
    {
        cv::Mat img_draw = convertTo3Channels(img);
        cv::Mat roi =convertToUint8(img);
        Ptr<CLAHE> clahe = createCLAHE(2.0, Size(10, 10));
        clahe->apply(roi, roi);
        cv::Mat srcbinary;
        Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
        //morphologyEx(roi, roi, MORPH_CLOSE, element, Point(-1, -1));
         cv::threshold(roi, srcbinary, 0, 255, CV_THRESH_OTSU);
       // cv::threshold(roi, srcbinary, 27, 255, CV_THRESH_BINARY);
        Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
        morphologyEx(srcbinary, srcbinary, MORPH_CLOSE, kernel, Point(-1, -1));
        cv::Mat srccanny;
        Canny(srcbinary, srccanny, 30, 90);
        vector<vector<Point>> contours;
        vector<Vec4i> hierachy;
        findContours(srcbinary, contours, hierachy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
        vector<Vec4i> hierachy_new;
        vector<cv::Point2f> pts;
        for (int i = 0; i < contours.size(); i++)
        {
            double area = contourArea(contours[i], false);
            if (area > 2e3/pow(m_binNum,2) && area < 4e4/pow(m_binNum,2))
            {
                // drawContours(img_draw, contours, i, Scalar(rand() * 255, rand() * 255, rand() * 255), 3, 8);
                 //RotatedRect rect = minAreaRect(contours[i]);
                 //rect.points(P);

                Rect rect = boundingRect(contours[i]);
                Point2f P[4];
                P[0] = cv::Point2f(rect.x, rect.y);
                P[1] = cv::Point2f(rect.x, rect.y + rect.height);
                P[2] = cv::Point2f(rect.x + rect.width, rect.y + rect.height);
                P[3] = cv::Point2f(rect.x + rect.width, rect.y);

                double w = Getdistance(P[0], P[1]);//abs(P[0].x - P[2].x);
                double h = Getdistance(P[0], P[3]);//abs(P[0].y - P[2].y);
                double asp = max(w, h) / min(w, h);
                if (w > 50/m_binNum /*& w < 200*/ & asp > 0.9 & asp < 1.2)
                {
 /*                   for (int j = 0; j <= 3; j++)
                    {
                        line(img_draw, P[j], P[(j + 1) % 4], Scalar(255, 0, 255), 2);
                    }*/
                    pts.push_back(P[0]);
                    pts.push_back(P[1]);
                    pts.push_back(P[2]);
                    pts.push_back(P[3]);
                }
            }
        }
        drawPointsOnImage(img_draw, pts, 1);

        if (pts.size() > 0)
        {
            // dian fen lei
            vector<cv::Point2f> ptsNew = pointsClusters(pts, 50/m_binNum);
            // test draw

            drawPointsOnImage(img_draw, ptsNew, 2,Scalar(0,255,0));


            cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, false, 50/m_binNum);
            MLOptParaChessboard cb;
            indexMap = cb.updateIndexMapKL(indexMap1, ptsNew, corners);
            if ((indexMap.rows * indexMap.cols) <= corners.size())
            {
                // draw
                for (int i = 0; i < indexMap.rows; i++)
                {
                    for (int j = 0; j < indexMap.cols; j++)
                    {
                        int in = indexMap.at<short>(i, j);
                        if (in < corners.size())
                        {
                            circle(img_draw, corners[in], 10, Scalar(255, 0, 255), -1);
                            // cout << corners[in] << endl;
                        }
                    }
                    //  cout << endl;
                }
            }
        }
        else
        {
            corners.clear();   
        }
        m_img_draw = img_draw.clone();
    }
    return corners;
}

vector<cv::Point2f> algorithm::MLGridDetect::GridTemplateGaussian(cv::Mat img, cv::Mat& indexMap)
{
    vector<cv::Point2f>corners;
    if (img.data != NULL)
    {
        cv::Mat img_draw = convertToUint8(img);
        cv::Mat img_draw1 = img_draw.clone();
        cv::Mat img_gray;
        if (img_draw.channels() == 3)
            cvtColor(img_draw, img_gray, CV_BGR2GRAY);
        else
            img_gray = img_draw.clone();
        cv::Mat templ;
        if (DEBUG == 1)
        {
          templ =
                cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\templateImg\\gridtemplate.tif", 0);

        }
        else
        {
         templ =
                cv::imread("./config/templateImg/gridtemplate.tif", 0);      
        }
        if (templ.empty())
            return vector<cv::Point2f>();
        cv::resize(templ, templ, templ.size() / m_binNum);
        CrossCenter c0;
        cv::Point2f templCenter = c0.find_centerGaussian(templ);
        cv::Mat img_result, img_result1, img_result2;
        matchTemplate(img_gray, templ, img_result, TM_CCOEFF_NORMED);
        cv::Mat imgre, imgre1, imgre2;
        //img_result.convertTo(img_result, CV_8UC1);
        //img_result = convertToUint8(img_result);
        threshold(img_result, imgre, 0.4, 255, CV_THRESH_BINARY);
        //threshold(img_result, imgre, 0, 255, CV_THRESH_OTSU);

        vector<cv::Point2f>pts, pts1, pts2;
        findNonZero(imgre, pts);

        vector<cv::Point2f>ptsNew;
        //TODO: 50
        ptsNew = pointsClusters(pts, 30);
        cv::Mat indexMap1 = generatePointsIndexMap(ptsNew, true, 30);
        MLOptParaChessboard cb;
        indexMap = cb.updateIndexMapKL(indexMap1, ptsNew, corners);
        // draw1
        for (int i = 0; i < corners.size(); i++)
        {
            circle(img_draw1, corners[i] + templCenter, 5, Scalar(255, 0, 255));
        }

        //  exact search, just select the peripheral points
        //TODO: 添加rect越界判断
        clock_t start, finish;
        start = clock();
        for (int i = 0; i < indexMap.cols; i++)
        {
            int in = indexMap.at<short>(i, 0);
            cv::Rect rect = cv::Rect(corners[in], templ.size());
            
            cv::Mat roi = img_gray(rect);
            CrossCenter cc(roi, 10);
           // cv::Point2f c0 = cc.find_centerGaussian(roi);
            cv::Point2f c0 = cc.find_centerLINES(roi);
            if (c0.x > 0 && c0.y > 0)
            {
                c0 = c0 + cv::Point2f(rect.x, rect.y);
                corners[in] = c0;
            } 
            circle(img_draw, c0, 1, Scalar(255, 0, 255), -1);

        }
        for (int i = 0; i < indexMap.cols; i++)
        {
            int in = indexMap.at<short>(i, indexMap.rows - 1);
            cv::Rect rect = cv::Rect(corners[in], templ.size());
            cv::Mat roi = img_gray(rect);
            CrossCenter cc(roi, 10);
           // cv::Point2f c0 = cc.find_centerGaussian(roi);
            cv::Point2f c0 = cc.find_centerLINES(roi);

       
            if (c0.x > 0 && c0.y > 0)
            {
                c0 = c0 + cv::Point2f(rect.x, rect.y);
                corners[in] = c0;
            }
            circle(img_draw, c0, 1, Scalar(255, 0, 255), -1);

        }
        for (int i = 1; i < indexMap.rows - 1; i++)
        {
            int in = indexMap.at<short>(0, i);
            cv::Rect rect = cv::Rect(corners[in], templ.size());
            cv::Mat roi = img_gray(rect);
            CrossCenter cc(roi, 10);
           // cv::Point2f c0 = cc.find_centerGaussian(roi);
            cv::Point2f c0 = cc.find_centerLINES(roi);

           
            if (c0.x > 0 && c0.y > 0)
            {
                c0 = c0 + cv::Point2f(rect.x, rect.y);
                corners[in] = c0;
            }
            circle(img_draw, c0, 1, Scalar(255, 0, 255), -1);
        }
        for (int i = 1; i < indexMap.rows - 1; i++)
        {
            int in = indexMap.at<short>(indexMap.cols - 1, i);
            cv::Rect rect = cv::Rect(corners[in], templ.size());
            cv::Mat roi = img_gray(rect);
            CrossCenter cc(roi, 10);
            //cv::Point2f c0 = cc.find_centerGaussian(roi);
            cv::Point2f c0 = cc.find_centerLINES(roi);
            if (c0.x > 0 && c0.y > 0)
            {
                c0 = c0 + cv::Point2f(rect.x, rect.y);
                corners[in] = c0;
            }
            circle(img_draw, c0, 1, Scalar(255, 0, 255), -1);
        }
        for (int i = 1; i < indexMap.rows - 1; i++)
        {
            for (int j = 1; j < indexMap.cols - 1; j++)
            {
                int in = indexMap.at<short>(i, j);
                corners[in] = corners[in] + templCenter;
            }
        }

        //draw
        for (int i = 0; i < indexMap.rows; i++)
        {
            for (int j = 0; j < indexMap.cols; j++)
            {
                int in = indexMap.at<short>(i, j);
                if (in < corners.size())
                {
                    circle(img_draw, corners[in], 5, Scalar(255, 0, 255), -1);
                }
            }
        }


        m_img_draw = img_draw.clone();
    }
    return corners;
}

cv::Mat algorithm::MLGridDetect::getImgDraw()
{
    return m_img_draw;
}
void algorithm::MLGridDetect::SetbinNum(int bin)
{
    m_binNum = bin;
}
