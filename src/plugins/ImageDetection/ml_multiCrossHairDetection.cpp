#include "ml_multiCrossHairDetection.h"
#include "CrossCenter.h"
#include "LogPlus.h"
#include<armadillo>
#include"ImageFilter.h"
using namespace cv;
using namespace MLImageDetection;

MLImageDetection::MultiCrossHairDetection::MultiCrossHairDetection()
{
}

MLImageDetection::MultiCrossHairDetection::~MultiCrossHairDetection()
{
}

MultiCrossHairRe MLImageDetection::MultiCrossHairDetection::getMuliCrossHairCenterByTemplate(cv::Mat img, int binNum)
{
    MultiCrossHairRe mlchRe;
    map<string, cv::Point2f> ptMap;
    map<string, cv::Rect> rectMap;

    string infoHeader = "---MLImageDetection log---MultiCrossHairDetectionALG---";
    if (img.data != NULL)
    {
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        cv::Mat gray = convertToGrayImage(img8);
        // cv::Mat templ = cv::imread("./config/ALGConfig/template41.tif",0);
        string filepath = get_exe_dir() + "/config/ALGConfig/crossTempl.tif";
        cv::Mat templ = cv::imread(filepath, 0);
        cv::resize(templ, templ, templ.size() / binNum);
        vector<cv::Point2f> P;
        P = findCrossHairCenterTemplate(img8, templ);
        vector<cv::Rect> rectVec;
        P = findCrossHairCenterContour(img8, rectVec, binNum);
        if (P.size() == 14)
        {
            cv::Point2f cen0 = getPointsMean(P);
            circle(img_draw, cen0, 5, Scalar(0, 0, 255));
            int maxlen = 600 / binNum;
            int minlen = 910 * sin(10 * CV_PI / 180) / binNum;
            for (int i = 0; i < P.size(); i++)
            {
                double dis = Getdistance(P[i], cen0);
                if (dis < 10)
                {
                    ptMap.insert(make_pair("center", P[i]));
                    rectMap.insert(make_pair("center", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(0, 0, 255), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(0, 0, 255), 2);
                    cv::putText(img_draw, "center", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
                float deltx = cen0.x - P[i].x;
                float delty = cen0.y - P[i].y;
                if (deltx > maxlen && delty > maxlen)
                {
                    ptMap.insert(make_pair("TL", P[i]));
                    rectMap.insert(make_pair("TL", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(0, 255, 0), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(0, 255, 0), 2);
                    cv::putText(img_draw, "TL", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
                else if (deltx > maxlen && abs(delty) < minlen)
                {
                    ptMap.insert(make_pair("ML", P[i]));
                    rectMap.insert(make_pair("ML", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(255, 0, 0), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(255, 0, 0), 2);
                    cv::putText(img_draw, "ML", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
                else if (deltx > maxlen && (delty) < -maxlen)
                {
                    ptMap.insert(make_pair("BL", P[i]));
                    rectMap.insert(make_pair("BL", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(255, 0, 255), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(255, 0, 255), 2);
                    cv::putText(img_draw, "BL", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
                else if (abs(deltx) < minlen && (delty) > maxlen)
                {
                    ptMap.insert(make_pair("TM", P[i]));
                    rectMap.insert(make_pair("TM", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(0, 255, 255), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(0, 255, 255), 2);
                    cv::putText(img_draw, "TM", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
                else if (abs(deltx) < minlen && (delty) < -maxlen)
                {
                    ptMap.insert(make_pair("BM", P[i]));
                    rectMap.insert(make_pair("BM", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(255, 255, 0), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(255, 255, 0), 2);
                    cv::putText(img_draw, "BM", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
                else if (deltx < -maxlen && (delty) > maxlen)
                {
                    ptMap.insert(make_pair("TR", P[i]));
                    rectMap.insert(make_pair("TR", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(100, 100, 0), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(100, 100, 0), 2);
                    cv::putText(img_draw, "TR", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
                else if (deltx < -maxlen && abs(delty) < minlen)
                {
                    ptMap.insert(make_pair("MR", P[i]));
                    rectMap.insert(make_pair("MR", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(0, 100, 0), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(0, 100, 0), 2);
                    cv::putText(img_draw, "MR", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
                else if (deltx < -maxlen && (delty) < -maxlen)
                {
                    ptMap.insert(make_pair("BR", P[i]));
                    rectMap.insert(make_pair("BR", rectVec[i]));
                    circle(img_draw, P[i], 5, Scalar(10, 100, 50), -1);
                    cv::rectangle(img_draw, rectVec[i], Scalar(10, 100, 50), 2);
                    cv::putText(img_draw, "BR", rectVec[i].tl(), 0, 8 / binNum, Scalar(0, 0, 255), 8 / binNum);
                }
            }
            mlchRe.centerMap = ptMap;
            mlchRe.rectMap = rectMap;
            mlchRe.img_draw = img_draw;
        }
        else
        {
            string message = infoHeader + "The multiCrossHair  center num is not nine";
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
            mlchRe.flag = false;
            mlchRe.errMsg = message;
        }
    }
    else
    {
        string message = infoHeader + "The input image is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
        mlchRe.flag = false;
        mlchRe.errMsg = message;
    }

    return mlchRe;
}

MLImageDetection::MultiCrossHairRe MLImageDetection::MultiCrossHairDetection::getMuliCrossHairCenter(cv::Mat img, int binNum, bool flag)
{
    MultiCrossHairRe mlchRe;
    map<string, cv::Point2f> ptMap;
    map<string, cv::Rect> rectMap;

    string infoHeader = "---MLImageDetection log---MultiCrossHairDetectionALG---";
    if (img.data != NULL)
    {
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        cv::Mat gray = convertToGrayImage(img8);
        // cv::Mat templ = cv::imread("./config/ALGConfig/template41.tif",0);
        //string filepath = get_exe_dir() + "./config/ALGConfig/crossTempl.tif";
       // cv::Mat templ = cv::imread(filepath, 0);
        //cv::resize(templ, templ, templ.size() / binNum);
        cv::Mat imgResize;
        cv::resize(img8, imgResize,img8.size()/binNum);
        vector<cv::Point2f> P;
        vector<cv::Rect> rectVec;
        P = findCrossHairCenterContour(imgResize, rectVec, binNum);
       // if(P.size()!=9)
       // P = findCrossHairCenterTemplate(imgResize, templ);
        if (P.size() == 9)
        {
            int len = m_len / m_binNum;
            vector<cv::Point2f>ptsSort = sortPoints(P);
            for (int i = 0; i < ptsSort.size(); i++)
            {
                cv::Point2f pt0 = ptsSort[i] * binNum;
                cv::Rect rect0(pt0.x - len / 2, pt0.y - len / 2, len, len);

                if (flag)
                    pt0 = getExactCrossCenter(img8, rect0);
                drawPointOnImage(img_draw, pt0);
                //drawRectOnImage(img_draw, rect0);
                putTextOnImage(img_draw, to_string(i + 1), pt0,16);
                ptMap.insert(std::make_pair(to_string(i + 1), pt0));
                rectMap.insert(std::make_pair(to_string(i + 1), rect0));
            }
            mlchRe.img_draw = img_draw;
            mlchRe.centerMap = ptMap;
            mlchRe.rectMap = rectMap;
        }
        else
        {
            string message = infoHeader + "The multiCrossHair  center num is not nine";
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
            mlchRe.flag = false;
            mlchRe.errMsg = message;
        }
    }
    else
    {
        string message = infoHeader + "The input image is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
        mlchRe.flag = false;
        mlchRe.errMsg = message;
    }

    return mlchRe;
}

MLImageDetection::MultiCrossHairRe MLImageDetection::MultiCrossHairDetection::getMultiCrossHairCenterBySlope(cv::Mat img,
                                                                                               vector<cv::Point2f> &re)
{
    MultiCrossHairRe mlchRe;
    map<string, cv::Point2f> ptMap;
    map<string, cv::Rect> rectMap;

    string infoHeader = "---MLImageDetection log---MultiCrossHairDetectionALG---";
    if (img.data != NULL)
    {
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        cv::Mat gray = convertToGrayImage(img8);
        CenterCrossHairRe cenRe = getCenterCrossHairCenter(img8);
        if (!cenRe.flag)
        {
            mlchRe.flag = cenRe.flag;
            mlchRe.errMsg = cenRe.errMsg;
            return mlchRe;
        }

        cv::Mat cenMat = img8(cenRe.rectCenter).clone();
        CrossCenter cc;
        cv::Point2f center = cc.find_centerLINES(cenMat);
        if (center.x > 1e-6 && center.y > 1e-6)
        {

            vector<double> Vline = cc.GetVerticalLine();
            vector<double> Hline = cc.GetHorizontalLine();
            int len = img.rows;
            int width = img.cols;

            int binNum = sqrt((9200 * 11264) / img.total());
            double dis = 927 / binNum;
            double kh = -Hline[0] / Hline[1];
            double kv = -Vline[0] / Vline[1];
            double thetaH = atan(kh);
            double thetaV = atan(kv);
            cv::Point2f p1 = center + cv::Point2f(cenRe.rectCenter.x, cenRe.rectCenter.y);
            double b1 = p1.y - kh * p1.x;
            double deltx1 = (dis * cos(thetaH));
            double delty1 = (dis * sin(thetaH));
            cv::Point2f p11, p12;
            p11 = p1 - cv::Point2f(deltx1, delty1);
            p12 = p1 + cv::Point2f(deltx1, delty1);
            double deltx2 = (dis * cos(thetaV));
            double delty2 = (dis * sin(thetaV));
            cv::Point2f p2, p21, p22, p3, p31, p32;
            p2 = p1 - cv::Point2f(deltx2, delty2);
            p21 = p2 - cv::Point2f(deltx1, delty1);
            p22 = p2 + cv::Point2f(deltx1, delty1);

            p3 = p1 + cv::Point2f(deltx2, delty2);
            p31 = p3 - cv::Point2f(deltx1, delty1);
            p32 = p3 + cv::Point2f(deltx1, delty1);
            circle(img_draw, p1, 5, Scalar(255, 0, 255), -1);
            circle(img_draw, p11, 5, Scalar(255, 0, 255), -1);
            circle(img_draw, p12, 5, Scalar(255, 0, 255), -1);
            circle(img_draw, p2, 5, Scalar(255, 0, 255), -1);
            circle(img_draw, p3, 5, Scalar(255, 0, 255), -1);
            circle(img_draw, p21, 5, Scalar(255, 0, 255), -1);
            circle(img_draw, p22, 5, Scalar(255, 0, 255), -1);
            circle(img_draw, p31, 5, Scalar(255, 0, 255), -1);
            circle(img_draw, p32, 5, Scalar(255, 0, 255), -1);
            if (p3.y < p2.y)
            {

                re.push_back(p31);
                re.push_back(p3);
                re.push_back(p32);
                re.push_back(p11);
                re.push_back(p1);
                re.push_back(p12);
                re.push_back(p21);
                re.push_back(p2);
                re.push_back(p22);
            }
            else
            {
                re.push_back(p21);
                re.push_back(p2);
                re.push_back(p22);
                re.push_back(p11);
                re.push_back(p1);
                re.push_back(p12);
                re.push_back(p31);
                re.push_back(p3);
                re.push_back(p32);
            }

            for (int i = 0; i < re.size(); i++)
            {

                int minIdx = findClosestPoint(cenRe.cenVec, re[i]);
                string str;
                switch (i)
                {
                case 0:
                    str = "TL";
                    break;
                case 1:
                    str = "TM";
                    break;
                case 2:
                    str = "TR";
                    break;
                case 3:
                    str = "ML";
                    break;
                case 4:
                    str = "center";
                    break;
                case 5:
                    str = "MR";
                    break;
                case 6:
                    str = "BL";
                    break;
                case 7:
                    str = "BM";
                    break;
                case 8:
                    str = "BR";
                    break;
                default:
                    break;
                }
                if (minIdx >= 0 && minIdx < cenRe.cenVec.size())
                {
                    ptMap.insert(make_pair(str, cenRe.cenVec[minIdx]));
                    rectMap.insert(make_pair(str, cenRe.rectVec[minIdx]));
                    circle(img_draw, cenRe.cenVec[minIdx], 5, Scalar(0, 255, 0), -1);
                    cv::rectangle(img_draw, cenRe.rectVec[minIdx], Scalar(0, 0, 255), 2);
                    cv::putText(img_draw, str, cenRe.rectVec[minIdx].tl(), 0, 8 / binNum, Scalar(0, 0, 255),
                                8 / binNum);
                }
            }

            mlchRe.centerMap = ptMap;
            mlchRe.rectMap = rectMap;
            mlchRe.img_draw = img_draw;
        }
        else
        {
            string message = infoHeader + "The center Line detect fail!!!";
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
            mlchRe.flag = false;
            mlchRe.errMsg = message;
            return mlchRe;
        }
    }
    else
    {
        string message = infoHeader + "The input image is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
        mlchRe.flag = false;
        mlchRe.errMsg = message;
        return mlchRe;
    }
    return mlchRe;
}

//MultiCrossHairRe MLImageDetection::MultiCrossHairDetection::getMuliCrossHairCenterByDistance(cv::Mat img, int binNum)
//{
//    MultiCrossHairRe mlchRe;
//    map<string, cv::Point2f> ptMap;
//    map<string, cv::Rect> rectMap;
//
//    string infoHeader = "---MLImageDetection log---getMuliCrossHairCenterByDistance---";
//    if (img.data != NULL)
//    {
//        m_binNum = binNum;
//        cv::Mat img8 = convertToUint8(img);
//        cv::Mat img_draw = convertTo3Channels(img8);
//        CenterCrossHairRe cenRe = getCenterCrossHairCenterByContour(img8, binNum);
//        if (!cenRe.flag.success)
//        {
//            mlchRe.flag = cenRe.flag;
//            return mlchRe;
//        }
//
//        cv::Mat cenMat = img8(cenRe.rectCenter).clone();
//        CrossCenter cc;
//        cv::Point2f center = cc.find_centerLINES(cenMat);
//        if (center.x > 1e-6 && center.y > 1e-6)
//        {
//            center = center + cv::Point2f(cenRe.rectCenter.tl());
//            // theta=67
//            // double theta = -cc.GetVerticalLineTheta();
//            // theta=-67
//            // double theta = -cc.GetVerticalLineTheta();
//
//            // for horizontal image
//            //  double theta = cc.GetHorizontalLineTheta();
//            // double theta1 = CV_PI / 2.0+cc.GetVerticalLineTheta();
//
//            double theta0 = cc.GetHorizontalLineTheta();
//            double theta;
//            if (abs(theta0) > 0.1)
//                theta = -cc.GetVerticalLineTheta();
//            else
//                theta = cc.GetHorizontalLineTheta();
//
//            int len = 400 / m_binNum;
//            int len2 = 200 / m_binNum;
//            vector<cv::Point2f> ptsVec;
//            for (int i = 0; i < 14; i++)
//            {
//                cv::Point2f pt0 = getPointByDeg(center, theta, xdeg0[i] / binNum, ydeg0[i] / binNum);
//                cv::Rect rect0 = cv::Rect(pt0.x - len / 2, pt0.y - len / 2, len, len);
//                if (i == 6 || i == 7)
//                {
//                    rect0 = cv::Rect(pt0.x - len2 / 2, pt0.y - len2 / 2, len2, len2);
//                }
//                drawRectOnImage(img_draw, rect0);
//                drawPointOnImage(img_draw, pt0);
//                ptsVec.push_back(center + cv::Point2f(xdeg0[i] / binNum, ydeg0[i] / binNum));
//                ptMap.insert(std::make_pair(to_string(i), pt0));
//                rectMap.insert(std::make_pair(to_string(i), rect0));
//                mlchRe.rectVec.push_back(rect0);
//            }
//            double angle = theta * 180.0 / CV_PI;
//            // another method to get the points
//            vector<cv::Point2f> ptsR = rotatePoints(ptsVec, center, angle);
//            drawPointsOnImage(img_draw, ptsR, 5, Scalar(0, 255, 0));
//
//            mlchRe.centerMap = ptMap;
//            mlchRe.rectMap = rectMap;
//            mlchRe.img_draw = img_draw;
//        }
//        else
//        {
//            string message = infoHeader + "The center Line detect fail!!!";
//            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
//            mlchRe.flag.success = false;
//            mlchRe.flag.errorMsg = message;
//            return mlchRe;
//        }
//    }
//    else
//    {
//        string message = infoHeader + "The input image is NULL";
//        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
//        mlchRe.flag.success = false;
//        mlchRe.flag.errorMsg = message;
//        return mlchRe;
//    }
//    return mlchRe;
//}

MultiCrossHairRe MLImageDetection::MultiCrossHairDetection::getMuliCrossHairCenterByDistance(cv::Mat img, int eyeLoc,
                                                                                      int binNum)
{
    MultiCrossHairRe mlchRe;
    map<string, cv::Point2f> ptMap;
    map<string, cv::Rect> rectMap;

    string infoHeader = "---MLImageDetection log---getMuliCrossHairCenterByDistance---";
    if (img.data != NULL)
    {
        m_binNum = binNum;
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = convertTo3Channels(img8);
        CenterCrossHairRe cenRe = getCenterCrossHairCenterByContour(img8, binNum);
        if (!cenRe.flag)
        {
            mlchRe.flag = cenRe.flag;
            mlchRe.errMsg = cenRe.errMsg;
            return mlchRe;
        }

        cv::Mat cenMat = img8(cenRe.rectCenter).clone();
        CrossCenter cc;
        cv::Point2f center = cc.find_centerLINES(cenMat);
        if (center.x > 1e-6 && center.y > 1e-6)
        {
            center = center + cv::Point2f(cenRe.rectCenter.tl());
            double theta = -cc.GetVerticalLineTheta();
            double theta0 = cc.GetHorizontalLineTheta();
            if (abs(theta0) > 0.2)
                theta = -cc.GetVerticalLineTheta();
            else
                theta = theta0;

            cv::Point2f d1;
            vector<double> xdeg, ydeg;

            if (abs(theta0) > 0.2)
            {
                if (eyeLoc == 0)
                {
                    d1 = getPointByDeg(center, theta, m_cenx / binNum, 0 / binNum);
                    xdeg = xdegL;
                    ydeg = ydegL;
                }
                else if (eyeLoc == 1)
                {
                    d1 = getPointByDeg(center, theta, -m_cenx / binNum, 0 / binNum);
                    xdeg = xdegR;
                    ydeg = ydegR;
                }
            }
            else
            {
               // theta0 = abs(theta0);
                if (eyeLoc == 1)
                {
                    d1 = getPointByDeg(center, theta, m_cenx / binNum, 0 / binNum);
                    xdeg = xdegL;
                    ydeg = ydegL;
                }
                else if (eyeLoc == 0)
                {
                    d1 = getPointByDeg(center, theta, -m_cenx / binNum, 0 / binNum);
                    xdeg = xdegR;
                    ydeg = ydegR;
                }
            }
            vector<cv::Point2f> ptsVec;
            for (int i = 0; i < ydeg.size(); i++)
            {
                ptsVec.push_back(d1 + Point2f(xdeg[i] / binNum, ydeg[i] / binNum));
            }
            double angle = theta * 180.0 / CV_PI;
            vector<cv::Point2f> ptsR = rotatePoints(ptsVec, d1, angle);
            drawPointsOnImage(img_draw, ptsR, 5, Scalar(0, 255, 0));

            int len = 500 / m_binNum;
            for (int i = 0; i < ptsR.size(); i++)
            {
                if (i == 0 || i == 9)
                    len = 250 / m_binNum;
                else
                    len = 450 / m_binNum;
                cv::Rect rect0 = cv::Rect(ptsR[i].x - len / 2, ptsR[i].y - len / 2, len, len);
                ptMap.insert(std::make_pair(to_string(i), ptsR[i]));
                rectMap.insert(std::make_pair(to_string(i), rect0));
                mlchRe.rectVec.push_back(rect0);
                drawRectOnImage(img_draw, rect0);
                putTextOnImage(img_draw, to_string(i), rect0.tl(),16/binNum); 
            }
           // drawRectsOnImage(img_draw, mlchRe.rectVec);
            mlchRe.centerMap = ptMap;
            mlchRe.rectMap = rectMap;
            mlchRe.img_draw = img_draw;
        }
        else
        {
            string message = infoHeader + "The center Line detect fail!!!";
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
            mlchRe.flag = false;
            mlchRe.errMsg = message;
            return mlchRe;
        }
    }
    else
    {
        string message = infoHeader + "The input image is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
        mlchRe.flag = false;
        mlchRe.errMsg = message;
        return mlchRe;
    }
    return mlchRe;
}

MLImageDetection::CenterCrossHairRe MLImageDetection::MultiCrossHairDetection::getCenterCrossHairCenter(cv::Mat img, int binNum)
{
    CenterCrossHairRe re;
    cv::Point2f center(0, 0);
    string infoHeader = "---MLImageDetection log---getCenterCrossHairCenter---";
    if (img.data != NULL)
    {
        cv::Mat img_draw = convertTo3Channels(img);
        cv::Mat gray = convertToGrayImage(img);
        vector<cv::Point2f> P;
        vector<cv::Rect> rectVec;
        P = findCrossHairCenterContour(gray, rectVec,binNum);
        if (P.size() == 14)
        {
            center = getPointsMean(P);
            re.center = center;
            re.cenVec = P;
            re.rectVec = rectVec;
            int minIdx = findClosestPoint(P, center);
            if (minIdx >= 0 && minIdx < rectVec.size())
                re.rectCenter = rectVec[minIdx];
            else
            {
                string message = infoHeader + "The CenterCrossHairCenter detection fail";
                LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
                re.flag = false;
                re.errMsg = message;
                return re;
            }
            circle(img_draw, re.center, 5, Scalar(0, 0, 255));
            cv::rectangle(img_draw, re.rectCenter, Scalar(0, 0, 255), 1);
            re.img_draw = img_draw;
            return re;
        }
        else
        {
            string message = infoHeader + "The multiCrossHair  center detection fail";
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
            re.flag = false;
            re.errMsg = message;
            return re;
        }
    }
    else
    {
        string message = infoHeader + "The input image is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
        re.flag = false;
        re.errMsg = message;
        return re;
    }
}

CenterCrossHairRe MLImageDetection::MultiCrossHairDetection::getCenterCrossHairCenterByContour(cv::Mat img, int binNum)
{

    CenterCrossHairRe re;
    cv::Point2f center(0, 0);
    string infoHeader = "---MLImageDetection log---getCenterCrossHairCenter---";
    if (img.data != NULL)
    {
        cv::Mat img_draw = convertTo3Channels(img);
        cv::Mat gray = convertToGrayImage(img);
        cv::Mat gray1 = gray.clone();
        Mat kernel = getStructuringElement(MORPH_RECT, Size(150 / binNum, 150 / binNum), Point(-1, -1));
       // morphologyEx(gray, gray, MORPH_DILATE, kernel, Point(-1, -1));
        cv::Mat imgth;
       // cv::threshold(gray, imgth, 0, 255, THRESH_OTSU);

        // test code
        Mat kernel1 = getStructuringElement(MORPH_RECT, Size(20 / binNum, 20 / binNum), Point(-1, -1));
        morphologyEx(gray1, gray1, MORPH_GRADIENT, kernel1, Point(-1, -1));
        Mat kernel2 = getStructuringElement(MORPH_RECT, Size(40 / binNum, 40 / binNum), Point(-1, -1));
        //  morphologyEx(gray1, gray1, MORPH_CLOSE, kernel2, Point(-1, -1));
        cv::Mat imgth1;
        cv::threshold(gray1, imgth, 0, 255, THRESH_TRIANGLE);
       // NaiveRemoveNoise(imgth, 8000/(binNum*binNum));
       // ContoursRemoveNoise(imgth, 8000 / (binNum * binNum));
        Clear_MicroConnected_Areas(imgth, imgth, 8000 / (binNum * binNum));
        morphologyEx(imgth, imgth, MORPH_CLOSE, kernel2, Point(-1, -1));

        // test end
        cv::Point2f c0(imgth.cols / 2, img.rows / 2);
        cv::Rect rect;
        // cv::RotatedRect rectR = getMaxAreaContourRect(imgth, rect);
        cv::RotatedRect rectR = getClosestContourRect(imgth, c0, rect);
        if (rectR.size.area() > 1e2)
        {
            center = rectR.center;
            re.center = center;
            re.rectCenter = rect;
            circle(img_draw, re.center, 5, Scalar(0, 0, 255));
            cv::rectangle(img_draw, re.rectCenter, Scalar(0, 0, 255), 1);
            re.img_draw = img_draw;
            return re;
        }
        else
        {
            string message = infoHeader + "The multiCrossHair  center detection fail";
            LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
            re.flag = false;
            re.errMsg = message;
            return re;
        }
    }
    else
    {
        string message = infoHeader + "The input image is NULL";
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, message.c_str());
        re.flag = false;
        re.errMsg = message;
        return re;
    }
}
void MLImageDetection::MultiCrossHairDetection::setBinNum(int num)
{
    m_binNum = num;;
}
cv::Mat MLImageDetection::MultiCrossHairDetection::getImgdraw()
{
    return m_img_draw;
}
vector<cv::Point2f> MLImageDetection::MultiCrossHairDetection::findCrossHairCenterContour(cv::Mat gray,
                                                                                   vector<cv::Rect> &rectVec,int binNum)
{
    vector<cv::Point2f> cenVec;
    cv::Mat img_draw = convertTo3Channels(gray);
    cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, cv::Size(10, 10));
    cv::morphologyEx(gray, gray, MORPH_CLOSE, kernel);
    cv::morphologyEx(gray, gray, MORPH_GRADIENT, kernel);

    //Ptr<CLAHE> clahe = createCLAHE(20, Size(20, 20));
    //clahe->apply(gray, gray);
    //ImageFilter filter;
    //filter.GaussianLowPassFilter(gray, gray);
    
    cv::Mat imgth;
    cv::threshold(gray, imgth, 0, 255, THRESH_TRIANGLE);
   // NaiveRemoveNoise(imgth, 100);
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<cv::Point2f> centers;

    //  drawContours(img_draw, contours, -1, Scalar(0, 0, 255), 10);
    for (int i = 0; i < contours.size(); i++)
    {
        double area = cv::contourArea(contours[i]);
        cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
        Point2f P[4];
        rectR.points(P);
        double w = Getdistance(P[0], P[1]);
        double h = Getdistance(P[1], P[2]);
        double ratio = min(w, h) / max(w, h);
        double areaRatio = area / (w * h);
        cv::Rect rect = cv::boundingRect(contours[i]);
        //int bin = sqrt(7920 * 6004 / gray.total());
        
      //  vector<cv::Point2f>cenVec;
        if (rect.area() > 10e4 / (binNum * binNum) && ratio > 0.5 && ratio < 1.4)
        {

            cv::Moments m = cv::moments(contours[i]);
            double cx = m.m10 / m.m00;
            double cy = m.m01 / m.m00;
            cv::Point2f cen(cx, cy);
            cenVec.push_back(cen);
           // cenVec.push_back(rectR.center);
            rectVec.push_back(rect);
            cv::rectangle(img_draw, rect, Scalar(0, 0, 255), 1);
            circle(img_draw, cen, 5, Scalar(0, 0, 255), -1);
            cv::line(img_draw, P[0], P[1], Scalar(0, 0, 255), 1);
            cv::line(img_draw, P[1], P[2], Scalar(0, 0, 255), 1);
            cv::line(img_draw, P[2], P[3], Scalar(0, 0, 255), 1);
            cv::line(img_draw, P[0], P[3], Scalar(0, 0, 255), 1);
        }
    }
    return cenVec;
}

vector<cv::Point2f> MLImageDetection::MultiCrossHairDetection::findCrossHairCenterTemplate(cv::Mat img, cv::Mat templ)
{
    vector<cv::Point2f> cenVec;
    if (img.data != NULL && templ.data != NULL)
    {
        cv::Mat gray, img_draw;
        gray = img;
 
        cv::Mat img_result;
        matchTemplate(gray, templ, img_result, TM_CCOEFF_NORMED);
        cv::Mat imgre;
        threshold(img_result, imgre, 0.7, 255, THRESH_BINARY);
        imgre.convertTo(imgre, CV_8UC1);
        vector<vector<Point>> contours;
        vector<Vec4i> hierachy;
        findContours(imgre, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        vector<Vec4i> hierachy_new;
        vector<double> areaVec;
        vector<cv::Point2f> ptVec;
        cv::Point2f templCenter(templ.rows / 2.0, templ.cols / 2.0);
        for (int i = 0; i < contours.size(); i++)
        {
            double area = contourArea(contours[i], false);
            cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
            cv::Rect rect = cv::boundingRect(contours[i]);
            areaVec.push_back(area);

            ptVec.push_back(rectR.center + templCenter);
            cv::drawContours(img_draw, contours, i, Scalar(0, 0, 255), 1);
        }
        vector<double> areaVec1 = areaVec;
        sort(areaVec.begin(), areaVec.end(), greater<double>());
        vector<double>::iterator iter;
        for (int i = 0; i < areaVec.size(); i++)
        {
            if (i > 8)
                break;
            iter = find(areaVec1.begin(), areaVec1.end(), areaVec[i]);
            int list = iter - areaVec1.begin();
            cenVec.push_back(ptVec[list]);
            circle(img_draw, ptVec[list], 5, Scalar(0, 255, 0), -1);
        }
    }
    return cenVec;
}

cv::Point2f MLImageDetection::MultiCrossHairDetection::getPointsMean(vector<cv::Point2f> pts)
{
    cv::Point2f ptM(0, 0);
    if (pts.size() == 0)
        return ptM;
    double sumx = 0, sumy = 0;
    for (int i = 0; i < pts.size(); i++)
    {
        sumx = sumx + pts[i].x;
        sumy = sumy + pts[i].y;
    }
    ptM.x = sumx / pts.size();
    ptM.y = sumy / pts.size();
    return ptM;
}

int MLImageDetection::MultiCrossHairDetection::findClosestPoint(vector<cv::Point2f> pts, cv::Point2f p0)
{
    vector<double> disVec;
    for (int i = 0; i < pts.size(); i++)
    {
        double dis = Getdistance(pts[i], p0);
        disVec.push_back(dis);
    }
    int minIdx = -1;
    minIdx = min_element(disVec.begin(), disVec.end()) - disVec.begin();
    return minIdx;
}

cv::Point2f MLImageDetection::MultiCrossHairDetection::getExactCrossCenter(cv::Mat img8, cv::Rect rect)
{
    cv::Mat roi = img8(rect).clone();
    CrossCenter cc;
    cv::Point2f  pt0 = cc.find_centerLINES(roi)+ Point2f(rect.tl());
    return pt0;
}

cv::Point2f MLImageDetection::MultiCrossHairDetection::getPointByDeg(cv::Point2f center, double theta, double xdeg,
                                                              double ydeg)
{
    cv::Point2f pt;
    double dis = sqrt(xdeg * xdeg + ydeg * ydeg);
    double deltx = xdeg * cos(theta) + ydeg * sin(theta);
    double delty = -xdeg * sin(theta) + ydeg * cos(theta);
    pt.x = center.x + deltx;
    pt.y = center.y + delty;
    return pt;
}

void MLImageDetection::MultiCrossHairDetection::updateCenterPoints(cv::Point2f &pt1, cv::Point2f &d1, double thetaH,
                                                            cv::Point2f center, int eyeLoc)
{
    // if (abs(theta0) > 0.2)
    //{

    //    if (eyeLoc == 0)
    //    {
    //        pt1 = getPointByDeg(center, theta, -m_cenx / binNum, 0 / binNum);
    //        d1 = getPointByDeg(center, theta, m_cenx / binNum, 0 / binNum);
    //    }
    //    else if (eyeLoc == 1)
    //    {
    //        pt1 = getPointByDeg(center, theta, m_cenx / binNum, 0 / binNum);
    //        d1 = getPointByDeg(center, theta, -m_cenx / binNum, 0 / binNum);
    //    }
    //}
    // else
    //{
    //    if (eyeLoc == 1)
    //    {
    //        pt1 = getPointByDeg(center, theta, -m_cenx / binNum, 0 / binNum);
    //        d1 = getPointByDeg(center, theta, m_cenx / binNum, 0 / binNum);
    //    }
    //    else if (eyeLoc == 0)
    //    {
    //        pt1 = getPointByDeg(center, theta, m_cenx / binNum, 0 / binNum);
    //        d1 = getPointByDeg(center, theta, -m_cenx / binNum, 0 / binNum);
    //    }
    //}
}

vector<cv::Point2f> MLImageDetection::MultiCrossHairDetection::sortPoints(vector<cv::Point2f> pts)
{
    arma::vec xvec(pts.size());
    arma::vec yvec(pts.size());
    for (int i = 0; i < pts.size(); i++)
    {
        xvec[i] = pts[i].x;
        yvec[i] = pts[i].y;
    }
   // arma::uvec xIndex = arma::sort_index(xvec);
    arma::uvec yIndex = arma::sort_index(yvec);
    vector<cv::Point2f>ptsSort;
    for (int i = 0; i < yIndex.size()-2; i=i+3)
    {
        arma::uvec yin0 = yIndex.subvec(i, i + 2);
        arma::vec xvec0 = xvec.elem(yin0);
        arma::uvec xin0 = arma::sort_index(xvec0);
        arma::uvec yin01 = yin0.elem(xin0);
        for (int j = 0; j < 3; j++)
        {
            cv::Point2f pt0;
            pt0.x = xvec[yin01[j]];
            pt0.y = yvec[yin01[j]];
            ptsSort.push_back(pt0);
        }
    }
    return ptsSort;
}
