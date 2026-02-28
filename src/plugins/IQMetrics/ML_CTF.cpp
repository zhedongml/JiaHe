#include "pch.h"
#include "ML_CTF.h"
using namespace MLImageDetection;
using namespace std;
using namespace MLIQMetrics;
using namespace cv;
MLIQMetrics::MLCTF::MLCTF()
{
}

MLIQMetrics::MLCTF::~MLCTF()
{
}

CTFRe MLIQMetrics::MLCTF::getCTF(cv::Mat img, CTFType type)
{
    string info = "------getCTF------";
    CTFRe re;
    if (img.empty())
    {
        re.flag = false;
        re.errMsg = info + "Input image is NULL";
        return re;
    }
    int binNum = IQMetricUtl::instance()->getBinNum(img.size());
    //img = img(IQMetricsParameters::ROIRect).clone();
    cv::Mat img8 = convertToUint8(img);
    cv::Mat imgdraw = convertTo3Channels(img8);
    
    cv::RotatedRect rectR;
    cv::Rect rect = getCTFRect(img8,rectR,imgdraw);
    cv::Mat roiMat = img(rect).clone();
    cv::Mat roidraw = imgdraw(rect);
    // TODO:  and the ctf ROI size
    double len = 200.0;
    int h = ceil(roiMat.rows / len);
    int w = ceil(roiMat.cols / len);
    cv::Mat ctfMap(Size(w, h), CV_32FC1);
    int i0 = 0;
    int j0 = 0;
    for (int i = 0; i < roiMat.rows; i=i+len)
    {
        for (int j = 0; j < roiMat.cols; j=j+len)
        {
            cv::Rect rect0(j, i, len, len);
            drawRectOnImage(roidraw, rect0);
            cv::Rect rectAnd = rect0 & cv::Rect(0, 0, roiMat.cols, roiMat.rows);
            cv::Mat roi0 = roiMat(rectAnd).clone();
            double ctf0 = calculateCTF(roiMat(rectAnd).clone(),type);
            ctfMap.at<float>(i0,j0) = ctf0;
            j0++;
        }
        i0++;
        j0 = 0;
    }
   
    return CTFRe();
}

AmazonCTFRe MLIQMetrics::MLCTF::getCTFAmazon(cv::Mat img, CTFType type)
{
    
        string info = "------getCTFAmazon------";
        AmazonCTFRe re;
        if (img.empty())
        {
            re.flag = false;
            re.errMsg = info + "Input image is NULL";
            return re;
        }
        cv::Rect ROIRect = IQMetricsParameters::ROIRect;
        int binNum = IQMetricUtl::instance()->getBinNum(img.size());
        img = GetROIMat(img, ROIRect);
        cv::Mat img8 = convertToUint8(img);
        cv::Mat imgdraw = convertTo3Channels(img8);
        map<string, cv::Point2f>cenMap;
        map<string, cv::Rect> rectMap = getCTFRectMap(img8, imgdraw, cenMap, binNum);
        map<string, double>ctfMap;
        // for the ALL ROI;
        //std::map<std::string, Rect>::iterator it;
        //for (it = rectMap.begin(); it != rectMap.end(); it++)
        //{
        //    string key = it->first;
        //    cv::Rect rect0 = it->second;
        //    cv::Mat roi = img(rect0).clone();
        //    double ctf0 = calculateCTF(roi, type);
        //    ctfMap.insert(std::make_pair(key, ctf0));
        //}
        std::map<std::string, cv::Point2f>::iterator it;
        for (it = cenMap.begin(); it != cenMap.end(); it++)
        {
            string key = it->first;
            cv::Point2f cen0 = it->second;
            int w = IQMetricsParameters::CTFROIWidth;
            int h = IQMetricsParameters::CTFROIHeight;
            cv::Rect rect0(cen0.x - w / 2.0, cen0.y - h / 2.0, w, h);
            cv::Mat roi = img(rect0).clone();
            double ctf0 = calculateCTF(roi, type);
            ctfMap.insert(std::make_pair(key, ctf0));
            drawRectOnImage(imgdraw, rect0);
            putTextOnImage(imgdraw, to_string(ctf0), rect0.br(), 10 / binNum);
        }

        re.ctfMap = ctfMap;
        re.imgdraw = imgdraw;
        return re;
    

}

double MLIQMetrics::MLCTF::calculateCTF(cv::Mat roi, CTFType type)
{
    cv::Mat rowMat;
    if (type == CTF_VERTICAL)
        cv::reduce(roi, rowMat, 1, REDUCE_AVG);
    else
        cv::reduce(roi, rowMat, 0, REDUCE_AVG);
    double minVal, maxVal;
    cv::minMaxLoc(rowMat, &minVal, &maxVal);
    double ctf0 = (maxVal - minVal) / (maxVal + minVal + 1e-6);  
    return ctf0;
}

cv::Rect MLIQMetrics::MLCTF::getCTFRect(cv::Mat img, cv::RotatedRect &rectR,cv::Mat& imgdraw)
{
    cv::Mat gray = convertToGrayImage(img);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(20,20));
    cv::morphologyEx(gray, gray, cv::MORPH_CLOSE,element);
    cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(gray, gray, cv::MORPH_GRADIENT, element1);
    cv::Mat imgth;
    cv::threshold(gray, imgth, 0, 255, cv::THRESH_OTSU);
    cv::morphologyEx(imgth, imgth, cv::MORPH_CLOSE, element);
    cv::Rect rect;
    rectR=getMaxAreaContourRect(imgth, rect);
    drawRectOnImage(imgdraw, rect);
    return rect;
}
map<string, cv::Rect> MLIQMetrics::MLCTF::getCTFRectMap(cv::Mat img, cv::Mat& imgdraw, map<string, cv::Point2f>& cenMap, int binNum)
{
    map<string, cv::Rect> rectMap;
    //map<string, cv::Point2f>cenMap;
    cv::Mat gray = convertToGrayImage(img);
    cv::Mat kernel = cv::getStructuringElement(MORPH_RECT, Size(30, 30));
    cv::morphologyEx(gray, gray, MORPH_CLOSE, kernel);
    cv::Mat imgth;
    cv::threshold(gray, imgth, 0, 255, THRESH_OTSU);
    vector<vector<Point>> contours;
    vector<Vec4i> hierachy;
    vector<cv::Point2f>cenVec;
    findContours(imgth, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<cv::Rect> rectVec;
    for (int i = 0; i < contours.size(); i++)
    {
        cv::Rect rect0 = cv::boundingRect(contours[i]);
        double area = cv::contourArea(contours[i]);
        cv::RotatedRect rectR = cv::minAreaRect(contours[i]);
        if (area > 10e4 / (binNum * binNum) && rect0.width > 300 && rect0.width < 380)
        {
            rectVec.push_back(rect0);
            cenVec.push_back(rectR.center);
            drawPointOnImage(imgdraw, rectR.center);
            drawRectOnImage(imgdraw, rect0);
        }
    }
    if (rectVec.size() != 17)
        return map<string, cv::Rect>();
    rectMap = sortRects(rectVec, cenVec, imgdraw, cenMap);
    return rectMap;
}

map<string, cv::Rect> MLIQMetrics::MLCTF::getCTFRectMapByDistance(cv::Mat img, cv::Mat& imgdra, int binNum)
{
    return map<string, cv::Rect>();
}

map<string, cv::Rect> MLIQMetrics::MLCTF::sortRects(vector<cv::Rect> rectVec, vector<cv::Point2f> cenVec, cv::Mat& imgdraw, map<string, cv::Point2f>& cenMap)
{
    map<string, cv::Rect> rectMap;
    // map<string, cv::Point2f>cenMap;
    arma::vec xv(cenVec.size());
    arma::vec yv(cenVec.size());
    cv::Point2f c0;
    for (int i = 0; i < cenVec.size(); i++)
    {
        xv[i] = cenVec[i].x;
        yv[i] = cenVec[i].y;
    }
    c0.x = arma::mean(xv);
    c0.y = arma::mean(yv);
    vector<double>disVec;
    arma::vec disvec(cenVec.size());
    arma::vec xsub = xv - c0.x;
    arma::vec ysub = yv - c0.y;
    disvec = arma::sqrt(xsub % xsub + ysub % ysub);
    circle(imgdraw, c0, 5, Scalar(0, 255, 0), -1);
    for (int i = 0; i < m_dis.size(); i++)
    {
        double dis0 = m_dis[i];
        arma::vec dissub = arma::abs(disvec - dis0);
        arma::uvec index = arma::find(dissub < 200);
        if (index.size() == 1)
        {
            int list = index[0];
            cv::Rect rect0 = rectVec[list];
            rectMap.insert(std::make_pair("Cen", rect0));
            cv::Point2f pt0(xv[list], yv[list]);
            cenMap.insert(std::make_pair("Cen", pt0));
            putTextOnImage(imgdraw, "Cen", rect0.tl(), 10);
            cv::rectangle(imgdraw, rect0, Scalar(0, 255, 0), 5);
            c0.x = xv[list];
            c0.y = yv[list];
        }
        else if (index.size() == 4)
        {
            for (int i0 = 0; i0 < index.size(); i0++)
            {

                double xdis = xv[index[i0]] - c0.x;
                double ydis = yv[index[i0]] - c0.y;
                cv::Rect rect0 = rectVec[index[i0]];
                cv::Point2f pt0(xv[index[i0]], yv[index[i0]]);
                string key;
                if (xdis < 0 && ydis < 0)
                {
                    key = to_string(i) + "_TL";
                    rectMap.insert(std::make_pair(to_string(i) + "_TL", rect0));
                    cenMap.insert(std::make_pair(key, pt0));
                }
                else if (xdis > 0 && ydis < 0)
                {
                    key = to_string(i) + "_TR";
                    rectMap.insert(std::make_pair(to_string(i) + "_TR", rect0));
                    cenMap.insert(std::make_pair(key, pt0));

                }
                else if (xdis < 0 && ydis>0)
                {
                    key = to_string(i) + "_BL";
                    rectMap.insert(std::make_pair(to_string(i) + "_BL", rect0));
                    cenMap.insert(std::make_pair(key, pt0));

                }
                else if (xdis > 0 && ydis > 0)
                {
                    key = to_string(i) + "_BR";
                    rectMap.insert(std::make_pair(to_string(i) + "_BR", rect0));
                    cenMap.insert(std::make_pair(key, pt0));
                }
                cv::rectangle(imgdraw, rect0, Scalar(0, 255, 0), 5);
                putTextOnImage(imgdraw, key, rect0.tl(), 10);
            }
        }

    }

    return rectMap;
}
