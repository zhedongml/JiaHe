#include "crossimgmtfmodel.h"
//#include "MLAlgorithm/ml_cross_center.h"
#include "mtfpipeline/pipeline.h"
#include <QFile>
#include <QDir>
//#include "MLAlgorithm/CrossCenter.h"
//#include"ml_cross_center.h"
//#include"CrossCenter.h"
#include"ImageDetection/CrossCenter.h"
CrossImgMTFModel::CrossImgMTFModel(QObject *parent) : QObject(parent)
{
}

CrossImgMTFModel::CrossImgMTFModel()
{
}

CrossImgMTFModel::~CrossImgMTFModel()
{
    m_roiVec.clear();
    clearResult();
}

bool CrossImgMTFModel::saveMTFData(QString path, QString fileName, int index)
{
    // calculateMTF();
    saveImageData(path, fileName, index);
    saveCSVData(path, fileName, index);
    return true;
}

void CrossImgMTFModel::calculateMTF(bool isAuto, QList<int> list)
{
    clearResult();
    QVector<CrossItemROI> roisVec;
    if (list.size() == 0)
    {
        roisVec = getCrossROI(isAuto);
    }
    else
    {
        roisVec = getCrossROI(list, isAuto);
    }
    if (roisVec.size() > 0)
    {
        std::vector<std::string> spcFrq = MTFModelConfig::instance()->getSpecifedFreq();
        focuslength = MTFModelConfig::instance()->getFocusLength();
        vid = MTFModelConfig::instance()->getVid();
        PipeLine *mtfPipeLine = new PipeLine();
        mtfPipeLine->set_freq_unit(FREQ_UNIT::lp_deg, focuslength);
        for (int i = 0; i < roisVec.size(); i++)
        {
            CrossItemROI roi = roisVec.at(i);
            cv::Rect rect(roi.fromx, roi.fromy, roi.width, roi.height);
            cv::Mat rawImg = crossImg(rect);

           bool flag= mtfROICalculateFlag(rawImg,i);
            if (flag == false)
                continue;
            cv::Mat roiImg;
            rawImg.copyTo(roiImg);
            ROIMTFValues *valItem = new ROIMTFValues;
            valItem->roiId = i;
            valItem->roiDir = roi.roiDir;
            // string name = "E:\\project\\hydrus2Imgs\\mtffail\\2\\roi01.tif";
            // imwrite(name,roiImg);
            int result = mtfPipeLine->culc_mtf(roiImg, ML_MTF_TYPE::CROSS);
            if (result == -1)
                valItem->result = -1;
            int size = mtfPipeLine->get_bin_len();
            valItem->binLen = size;
            if (size > 0 && result != -1)
            {

                /// lsf region
                double *lsffreq = mtfPipeLine->get_esf_lsf_x();
                double *lsf = mtfPipeLine->get_lsf();
                double *lsffreqCopy = new double[size * 2];
                memset(lsffreqCopy, 0, size * 2 * sizeof(double));
                memcpy(lsffreqCopy, lsffreq, size * 2 * sizeof(double));
                double *lsfCopy = new double[size * 2 * sizeof(double)];
                memset(lsfCopy, 0, size * 2 * sizeof(double));
                memcpy(lsfCopy, lsf, size * 2 * sizeof(double));
                valItem->m_lsfreqVals = lsffreqCopy;
                valItem->m_lsfVals = lsfCopy;
                /// lsf region

                /// mtf region
                double *mtffreq = mtfPipeLine->get_freq();
                double *mtf = mtfPipeLine->get_mtf();
                double *freqCopy = new double[size];
                memset(freqCopy, 0, size * sizeof(double));
                memcpy(freqCopy, mtffreq, size * sizeof(double));
                double *mtfCopy = new double[size];
                memset(mtfCopy, 0, size * sizeof(double));
                memcpy(mtfCopy, mtf, size * sizeof(double));
                valItem->m_freqVals = freqCopy;
                valItem->m_mtfVals = mtfCopy;
                /// mtf region
                for each (std::string var in spcFrq)
                {
                    // QString key = QString::number(fixFreq, 'f', 3);
                    QString key = QString::fromStdString(var);
                    double fixFreqVal = mtfPipeLine->getMtfByFreq(key.toFloat());
                    valItem->specMtfVal.insert(key, fixFreqVal);
                }
            }
            m_mtfValsVec.append(valItem);
        }
        delete mtfPipeLine;
    }
}

void CrossImgMTFModel::setImage(cv::Mat img)
{
    crossImg = img;
}

cv::Mat CrossImgMTFModel::getImage()
{
    return crossImg;
}

void CrossImgMTFModel::setAOIId(int val)
{
    aoiId = val;
}

int CrossImgMTFModel::getAOIId()
{
    return aoiId;
}

QVector<CrossItemROI> CrossImgMTFModel::getCrossROI(bool isAuto, int num)
{
    m_roiVec.clear();
    if (crossImg.empty())
        return m_roiVec;
    roiPosCfg = MTFModelConfig::instance()->getROIPosCfg();
    // std::vector<cv::Point2f> rois_center = my_cross.GetROICenter(roiPosCfg.centerOffset);
    MLImageDetection::CrossCenter crossCenter(crossImg, roiPosCfg.centerOffset);
    // crossCenter.get_crossCenter(1);
    cv::Point2f cen = crossCenter.find_centerLINES(crossImg);
    if (cen.x<1e-6||cen.y<1e-6)
    {
        cen = crossCenter.find_centerGaussian(crossImg);
    }
    vector<Point> rois_center = crossCenter.get_roi_center(); // crossCenter.get_newroi_center();

    if (rois_center.size() > 0)
    {

        switch (num)
        {
        case 1: {
            cv::Point2f roiCenter = rois_center.at(0);
            addOneROI(roiCenter, 0, "V");
            break;
        }
        case 2: {
            cv::Point2f roiCenter = rois_center.at(0);
            addOneROI(roiCenter, 0, "H");
            cv::Point2f roiCenter2 = rois_center.at(2);
            addOneROI(roiCenter2, 1, "V");
            break;
        }
        default: {
            for (int i = 0; i < rois_center.size(); i++)
            {
                cv::Point2f roiCenter = rois_center.at(i);
                if (i < 2)
                {
                    addOneROI(roiCenter, i, "V");
                }
                else
                {
                    addOneROI(roiCenter, i, "H");
                }
            }
            break;
        }
        }
    }
    return m_roiVec;
}

QVector<CrossItemROI> CrossImgMTFModel::getRectangleROI(int num)
{
    

        m_roiVec.clear();
        if (crossImg.empty())
            return m_roiVec;
        roiPosCfg = MTFModelConfig::instance()->getROIPosCfg(); //mtfconfig.json->ROIPostion
        //vector<cv::Point> rois_center = chessboardROI(crossImg);
        vector<cv::Point> rois_center = RectangleROI(crossImg); //找到方格每条边中心坐标，左，右，上，下
        //if (rois_center.size() < 4)
        //{
        //    rois_center.clear();
        //    cv::Point c0(crossImg.cols / 2, crossImg.rows / 2);

        //    rois_center.push_back(Point(c0.x - 113, c0.y));
        //    rois_center.push_back(Point(c0.x + 113, c0.y));
        //    rois_center.push_back(Point(c0.x, c0.y - 113));
        //    rois_center.push_back(Point(c0.x, c0.y + 113));
        //    //
        //    cv::Mat imgdraw = crossImg.clone();
        //    imgdraw.convertTo(imgdraw, CV_8UC1);
        //    circle(imgdraw, rois_center[0], 5, Scalar(255, 0, 255), -1);
        //    circle(imgdraw, rois_center[1], 5, Scalar(255, 0, 255), -1);
        //    circle(imgdraw, rois_center[2], 5, Scalar(255, 0, 255), -1);
        //    circle(imgdraw, rois_center[3], 5, Scalar(255, 0, 255), -1);

        //}

        if (rois_center.size() > 0)
        {

            switch (num)
            {
            case 1: {
                cv::Point2f roiCenter = rois_center.at(0);
                addOneROI(roiCenter, 0, "V");
                break;
            }
            case 2: {
                cv::Point2f roiCenter = rois_center.at(0);
                addOneROI(roiCenter, 0, "H");
                cv::Point2f roiCenter2 = rois_center.at(2);
                addOneROI(roiCenter2, 1, "V");
                break;
            }
            default: {
                for (int i = 0; i < rois_center.size(); i++)
                {
                    cv::Point2f roiCenter = rois_center.at(i);
                    if (i < 2)
                    {
                        addOneROI(roiCenter, i, "V");
                    }
                    else
                    {
                        addOneROI(roiCenter, i, "H");
                    }
                }
                break;
            }
            }
        }
        return m_roiVec;
    

}

QVector<cv::Rect> CrossImgMTFModel::getCrossROIFour(cv::Mat rectImg)
{
    if (rectImg.empty())
        return QVector<cv::Rect>();

    roiPosCfg = MTFModelConfig::instance()->getROIPosCfg();
  //  algorithm::MLCrossCenter my_cross;
    MLImageDetection::CrossCenter my_cross;
    MLImageDetection::CrossCenter crossCenter(rectImg, roiPosCfg.centerOffset);
    crossCenter.find_centerGaussian(rectImg);
    vector<Point> rois_center = crossCenter.get_roi_center(); // crossCenter.get_newroi_center();

    QVector<cv::Rect> roiVec;
    for (int i = 0; i < rois_center.size(); i++)
    {
        cv::Point2f roiCenter = rois_center.at(i);
        cv::Rect imgRect(0, 0, rectImg.cols, rectImg.rows);
        cv::Rect roi(roiCenter.x - roiPosCfg.roiWidth / 2, roiCenter.y - roiPosCfg.roiHeight / 2, roiPosCfg.roiWidth,
                     roiPosCfg.roiHeight);
        roi = roi & imgRect;
        roiVec.append(roi);
    }
    return roiVec;
}

QVector<CrossItemROI> CrossImgMTFModel::getCrossROI(QList<int> list, bool isAuto)
{
    m_roiVec.clear();
    if (list.size() == 0 || crossImg.empty())
        return m_roiVec;
    roiPosCfg = MTFModelConfig::instance()->getROIPosCfg();
   // algorithm::MLCrossCenter my_cross;
    MLImageDetection::CrossCenter my_cross;
    //CrossCenter my_cross;

    // cv::Point2f center = my_cross.GetCrossCenter(crossImg);
    // std::vector<cv::Point2f> rois_center = my_cross.GetROICenter(roiPosCfg.centerOffset);
    MLImageDetection::CrossCenter crossCenter(crossImg, roiPosCfg.centerOffset);
    // crossCenter.get_crossCenter(2); TODO:Borderless crosshair detection
    if (isAuto)
    {
        //  crossCenter.get_crossCenterMTF();
        crossCenter.find_centerMTF(crossImg);
    }
    else
    {
        crossCenter.find_centerMTF_manual(crossImg);
    }
    vector<Point> rois_center = crossCenter.get_roi_center(); // crossCenter.get_newroi_center();

    if (rois_center.size() > 0)
    {
        for (int num : list)
        {
            if (num < 2)
            {
                cv::Point2f roiCenter = rois_center.at(num);
                addOneROI(roiCenter, num, "V");
            }
            else
            {
                cv::Point2f roiCenter = rois_center.at(num);
                addOneROI(roiCenter, num, "H");
            }
        }
    }
    return m_roiVec;
}

cv::Mat CrossImgMTFModel::cvMatTo8Bit(const cv::Mat &intImg)
{
    cv::Mat img2 = intImg.clone();
    cv::Mat result = img2;
    if (img2.type() == CV_16UC1)
    {
        double minv = 0.0, maxv = 0.0;
        double *minp = &minv;
        double *maxp = &maxv;
        cv::minMaxIdx(img2, minp, maxp);

        if (maxv > 4095.1)
        {
            cv::Mat tmp = img2 / 65535 * 255;

            tmp.convertTo(result, CV_8UC1);
        }
        else
        {
            cv::Mat tmp = img2 / 4095.0 * 255;

            tmp.convertTo(result, CV_8UC1);
        }
    }
    else if (img2.type() == CV_32FC1)
    {
        double minv = 0.0, maxv = 0.0;
        double *minp = &minv;
        double *maxp = &maxv;
        cv::minMaxIdx(img2, minp, maxp);
        cv::Mat outtmp = img2 - minv;
        outtmp.convertTo(result, CV_8U, 255.0 / (maxv - minv));
    }
    return result;
}

void CrossImgMTFModel::addOneROI(cv::Point2f roiCenter, int id, QString dir)
{
    cv::Rect imgRect(0, 0, crossImg.cols, crossImg.rows);
    cv::Rect roi(roiCenter.x - roiPosCfg.roiWidth / 2, roiCenter.y - roiPosCfg.roiHeight / 2, roiPosCfg.roiWidth,
                 roiPosCfg.roiHeight);
    roi = roi & imgRect;
    CrossItemROI item;
    item.id = id;
    item.fromx = roi.x;
    item.fromy = roi.y;
    item.width = roi.width;
    item.height = roi.height;
    item.roiDir = dir;
    m_roiVec.append(item);
}

void CrossImgMTFModel::saveImageData(QString path, QString fileName, int index)
{
    if (crossImg.empty())
    {
        return;
    }
    QString imgFile = path + QDir::separator() + fileName + "_Cross_" + QString::number(index) + ".tif";
    std::string stdImgFileName = imgFile.toStdString();
    cv::Mat resultImg = cvMatTo8Bit(crossImg);
    QVector<CrossItemROI>::Iterator iter;
    for (iter = m_roiVec.begin(); iter != m_roiVec.end(); iter++)
    {
        CrossItemROI item = *iter;
        int i = iter - m_roiVec.begin();
        cv::rectangle(resultImg, cv::Rect(item.fromx, item.fromy, item.width, item.height), (255, 255, 255), 1, 4);
        cv::putText(resultImg, std::to_string(i) + "#",
                    cv::Point2d(item.fromx + item.width, item.fromy + int(item.height / 2)), cv::FONT_HERSHEY_SIMPLEX,
                    1.0, (255, 255, 255), 2);
    }
    cv::imwrite(stdImgFileName.c_str(), resultImg);
}

void CrossImgMTFModel::saveCSVData(QString path, QString fileName, int index)
{
    int maxLen = 0;
    int maxLenIdx = 0;
    QString lsfHeader = "Freq,";
    QString mtfHeader = "Freq,";
    QVector<ROIMTFValues *>::iterator iter;

    for (iter = m_mtfValsVec.begin(); iter != m_mtfValsVec.end(); iter++)
    {
        ROIMTFValues *item = *iter;
        int index = iter - m_mtfValsVec.begin();
        QString strindex = QString::number(index);
        lsfHeader += "LSF" + strindex + ",";
        mtfHeader += "MTF" + strindex + ",";
        if (item->binLen > maxLen)
        {
            maxLen = item->binLen;
            maxLenIdx = index;
        }
    }
    QString lsfFile = path + QDir::separator() + fileName + "_LSF_" + QString::number(index) + ".csv";
    QFile lsfDataFile(lsfFile);
    lsfDataFile.open(QIODevice::WriteOnly | QIODevice::Text);
    lsfDataFile.write((lsfHeader + "\n").toUtf8());
    for (int i = 0; i < maxLen * 2; i++)
    {
        QString csvitem = QString::number(m_mtfValsVec.at(maxLenIdx)->m_lsfreqVals[i]);
        for (iter = m_mtfValsVec.begin(); iter != m_mtfValsVec.end(); iter++)
        {
            ROIMTFValues *item = *iter;
            if (item->binLen * 2 >= i && item->result != -1)
            {
                csvitem = csvitem + "," + QString::number(item->m_lsfVals[i]);
            }
            else
            {
                csvitem = csvitem + ", ";
            }
        }
        lsfDataFile.write((csvitem + "\n").toUtf8());
    }
    lsfDataFile.close();
    QString mtfFile = path + QDir::separator() + fileName + "_MTF_" + QString::number(index) + ".csv";
    QFile mtfDataFile(mtfFile);
    mtfDataFile.open(QIODevice::WriteOnly | QIODevice::Text);
    mtfDataFile.write((mtfHeader + "\n").toUtf8());
    for (int i = 0; i < maxLen; i++)
    {
        QString csvitem = QString::number(m_mtfValsVec.at(maxLenIdx)->m_freqVals[i]);
        for (iter = m_mtfValsVec.begin(); iter != m_mtfValsVec.end(); iter++)
        {
            ROIMTFValues *item = *iter;
            if (item->binLen >= i && item->result != -1)
            {
                csvitem = csvitem + "," + QString::number(item->m_mtfVals[i]);
            }
            else
            {
                csvitem = csvitem + ", ";
            }
        }
        mtfDataFile.write((csvitem + "\n").toUtf8());
    }
    mtfDataFile.close();

    // save roi
    QString roiFile = path + QDir::separator() + fileName + "_ROI_" + QString::number(index) + ".csv";
    QFile roiDataFile(roiFile);
    roiDataFile.open(QIODevice::WriteOnly | QIODevice::Text);
    roiDataFile.write("id,fromx,fromy,width,height\n");
    QVector<CrossItemROI>::Iterator iter1;
    for (iter1 = m_roiVec.begin(); iter1 != m_roiVec.end(); iter1++)
    {
        CrossItemROI roiInfo = *iter1;
        QString str = QString::number(roiInfo.id) + ",";
        str.append(QString::number(roiInfo.fromx) + ",")
            .append(QString::number(roiInfo.fromy) + ",")
            .append(QString::number(roiInfo.width) + ",")
            .append(QString::number(roiInfo.height) + "\n");
        roiDataFile.write(str.toUtf8());
    }
    roiDataFile.close();
    // save foculeng vid
    QString camInfoFile = path + QDir::separator() + fileName + "_Camera_" + QString::number(index) + ".csv";
    QFile camInfoDataFile(camInfoFile);
    camInfoDataFile.open(QIODevice::WriteOnly | QIODevice::Text);
    camInfoDataFile.write("FocusLength,Vid\n");
    QString str = QString::number(focuslength) + "," + QString::number(vid);
    camInfoDataFile.write(str.toUtf8());
    camInfoDataFile.close();
}

bool CrossImgMTFModel::mtfROICalculateFlag(cv::Mat img, int i)
{
    cv::Mat img8 = cvMatTo8Bit(img);
    cv::Scalar m0 = cv::mean(img8);
    cv::Mat imgth;
    cv::threshold(img8, imgth, 0, 255, THRESH_OTSU);
    cv::Mat rowMat;
    if (i>1)
        cv::reduce(imgth, rowMat, 0, REDUCE_AVG);
    else 
    cv::reduce(imgth, rowMat, 1, REDUCE_AVG);

    int num = cv::countNonZero(rowMat);
    if (num < rowMat.total() * 2.0 / 3.0)
        return false;
    else
        return true;
}
Point2f getIntersection(Point2f p1, Point2f P0, Point2f P3, Point2f pt1, Point2f pt2)
{
    Point2f in;
    double k = (P0.y - P3.y) / (P0.x - P3.x);
    double k1 = -1 / k;
    double b1 = p1.y - k1 * p1.x;
    double k2;
    if (abs(pt1.x - pt2.x) > 1e-6)
        k2 = (pt1.y - pt2.y) / (pt1.x - pt2.x);
    else
        k2 = 1e6;
    double b2 = pt1.y - k2 * pt1.x;
    in.x = (b2 - b1) / (k1 - k2);
    in.y = k2 * in.x + b2;
    return in;
}

cv::Point2f CrossImgMTFModel::getLineAndRectIntersectionPoint(cv::Point2f p1, cv::Point2f P0, cv::Point2f P3, cv::Rect rect)
{ 
        cv::Point pt;
        cv::Point2f pt1, pt2, pt3, pt4;
        pt1 = cv::Point2f(rect.x, rect.y);
        pt2 = cv::Point2f(rect.x + rect.width, rect.y);
        pt3 = cv::Point2f(rect.x + rect.width, rect.y + rect.height);
        pt4 = cv::Point2f(rect.x, rect.y + rect.height);
        MLImageDetection::MLimagePublic pl;
        Point2f in1 = getIntersection(p1, P0, P3, pt1, pt2);
        double dis1 = pl.Getdistance(in1, p1);
        Point2f in2 = getIntersection(p1, P0, P3, pt2, pt3);
        double dis2 = pl.Getdistance(in2, p1);
        Point2f in3 = getIntersection(p1, P0, P3, pt3, pt4);
        double dis3 = pl.Getdistance(in3, p1);
        Point2f in4 = getIntersection(p1, P0, P3, pt1, pt4);
        double dis4 = pl.Getdistance(in4, p1);
        vector<double> disVec;
        disVec.push_back(dis1);
        disVec.push_back(dis2);
        disVec.push_back(dis3);
        disVec.push_back(dis4);
        vector<cv::Point2f> inVec;
        inVec.push_back(in1);
        inVec.push_back(in2);
        inVec.push_back(in3);
        inVec.push_back(in4);
        int minIndex = min_element(disVec.begin(), disVec.end()) - disVec.begin();
        Point2f in = inVec[minIndex];
        return in;
    
}

std::vector<cv::Point> CrossImgMTFModel::RectangleROI(cv::Mat img)
{
        vector<cv::Point> center;
        MLImageDetection::MLimagePublic pl;
        cv::Mat img_gray;
        if (img.channels() == 3)
            cvtColor(img, img_gray, COLOR_BGR2GRAY);
        else
            img_gray = img.clone();
        cv::Mat img8 = pl.convertToUint8(img_gray);
        cv::Mat img_draw = pl.convertTo3Channels(img8);
        Ptr<CLAHE> clahe = createCLAHE(2.0, Size(10, 10));
        //clahe->apply(img_gray, img_gray);
        Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
        //morphologyEx(img_gray, img_gray, MORPH_GRADIENT, kernel, Point(-1, -1));    cv::Mat imgth;
        cv::Mat imgth;
        cv::threshold(img8, imgth, 0, 255, CV_THRESH_OTSU);
        //MLimagePublic pl;
        //pl.NaiveRemoveNoise(imgth, 1000);
       // pl.ContoursRemoveNoise(imgth, 1000);
        cv::Mat elemet = getStructuringElement(MORPH_RECT, Size(5, 5));
        // cv::erode(imgth, imgth, elemet);
        std::vector<std::vector<cv::Point>> contours, contoursNew;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(imgth, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
        vector<double> areaVec;
        for (int i = 0; i < contours.size(); i++)
        {
            double area = contourArea(contours[i], false);
            areaVec.push_back(area);
            cv::Rect rect = boundingRect(contours[i]);
            //cv::rectangle(img_draw, rect, Scalar(0, 0, 255), 1);
            double ratio = double(min(rect.height, rect.width)) / max(rect.height, rect.width);
            if (area > 900 && area < 6e4 && ratio > 0.8 && ratio < 1.2)
            {
                cv::Rect contourRect = boundingRect(contours[i]);
                //  cv::rectangle(img_draw, contourRect, Scalar(0, 255, 0), 1);
                RotatedRect minRect = minAreaRect(contours[i]);
                cv::Point2f P[4];
                minRect.points(P);
                Point p1 = (P[0] + P[3]) / 2;
                Point p2 = (P[1] + P[2]) / 2;
                Point p3 = (P[0] + P[1]) / 2;
                Point p4 = (P[2] + P[3]) / 2;
                cv::line(img_draw, P[0], P[3], Scalar(0, 255, 0), 2);
                cv::line(img_draw, P[0], P[1], Scalar(0, 255, 0), 2);
                cv::line(img_draw, P[1], P[2], Scalar(0, 255, 0), 2);
                cv::line(img_draw, P[2], P[3], Scalar(0, 255, 0), 2);
                if (!contourRect.contains(p1))
                    p1 = getLineAndRectIntersectionPoint(p1, P[0], P[3], rect);
                if (!contourRect.contains(p2))
                    p2 = getLineAndRectIntersectionPoint(p2, P[1], P[2], rect);
                if (!contourRect.contains(p3))
                    p3 = getLineAndRectIntersectionPoint(p3, P[0], P[1], rect);
                if (!contourRect.contains(p4))
                    p4 = getLineAndRectIntersectionPoint(p4, P[2], P[3], rect);
                cv::circle(img_draw, p1, 5, Scalar(0, 0, 255), -1);
                cv::circle(img_draw, p2, 5, Scalar(0, 0, 255), -1);
                cv::circle(img_draw, p3, 5, Scalar(0, 0, 255), -1);
                cv::circle(img_draw, p4, 5, Scalar(0, 0, 255), -1);
                if (minRect.angle > 45)
                {
                    center.push_back(p1);
                    center.push_back(p2);
                    center.push_back(p3);
                    center.push_back(p4);
                }
                else
                {
                    center.push_back(p3);
                    center.push_back(p4);
                    center.push_back(p2);
                    center.push_back(p1);
                }


                // add draw
                cv::Rect rect1(p1.x - 184 / 2, p1.y - 138 / 2, 184, 138);
                cv::Rect rect2(p2.x - 184 / 2, p2.y - 138 / 2, 184, 138);
                cv::Rect rect3(p3.x - 138 / 2, p3.y - 184 / 2, 138, 184);
                cv::Rect rect4(p4.x - 138 / 2, p4.y - 184 / 2, 138, 184);
                cv::rectangle(img_draw, rect1, Scalar(0, 0, 255), 1);
                cv::rectangle(img_draw, rect2, Scalar(0, 0, 255), 1);
                cv::rectangle(img_draw, rect3, Scalar(0, 0, 255), 1);
                cv::rectangle(img_draw, rect4, Scalar(0, 0, 255), 1);

            }
        }
        if (center.size() > 4)
            center.clear();
        return center;
    

}

void CrossImgMTFModel::clearResult()
{
    QVector<ROIMTFValues *>::iterator iter;
    for (iter = m_mtfValsVec.begin(); iter != m_mtfValsVec.end(); iter++)
    {
        ROIMTFValues *item = *iter;
        delete item;
    }
    m_mtfValsVec.clear();
}

PassResult *CrossImgMTFModel::judgeResult()
{
    PassResult *result = new PassResult;
    result->aoiId = aoiId;
    std::vector<MTFLimitConfig> limits = MTFModelConfig::instance()->getMTFLimit(aoiId);
    ROIMTFValues *mtfval = nullptr;
    QVector<PassDetail> details;
    QVector<int> vDirMtfVals;
    QVector<int> hDirMtfVals;
    for (int i = 0; i < m_mtfValsVec.size(); i++)
    {
        mtfval = m_mtfValsVec.at(i);
        if (mtfval->roiDir == "H")
        {
            hDirMtfVals.append(i);
        }
        else if (mtfval->roiDir == "V")
        {
            vDirMtfVals.append(i);
        }
    }

    const QMap<QString, double> &specVals = mtfval->specMtfVal;

    for each (MTFLimitConfig limit in limits)
    {
        PassDetail vDetailItem;
        vDetailItem.dir = "V";
        vDetailItem.lowLimit = limit.lowerLimit;
        vDetailItem.upLimit = limit.upperLimit;
        vDetailItem.freq = QString::fromStdString(limit.freqVal);
        double vMtfVal = 0;
        double sumVMTF = 0;
        int num = 0;
        for each (int idx in vDirMtfVals)
        {
            mtfval = m_mtfValsVec.at(idx);
            const QMap<QString, double> &specVals = mtfval->specMtfVal;
            double val = specVals.value(vDetailItem.freq);
            if (val >= 0)
            {
                sumVMTF += val;
                num++;
            }
        }
        if (num > 0)
        {
            vDetailItem.mtfVal = sumVMTF / num;
        }
        else
        {
            vDetailItem.mtfVal = 0;
        }
        details.append(vDetailItem);
        double sumHMTF = 0;
        num = 0;
        PassDetail hDetailItem;
        hDetailItem.dir = "H";
        hDetailItem.lowLimit = limit.lowerLimit;
        hDetailItem.upLimit = limit.upperLimit;
        hDetailItem.freq = QString::fromStdString(limit.freqVal);
        for each (int idx in hDirMtfVals)
        {
            mtfval = m_mtfValsVec.at(idx);
            const QMap<QString, double> &specVals = mtfval->specMtfVal;
            double val = specVals.value(hDetailItem.freq);
            if (val > 0)
            {
                sumHMTF += val;
                num++;
            }
        }
        hDetailItem.mtfVal = sumHMTF / num;
        details.append(hDetailItem);
    }
    result->isPass = judgePassOrFail(details);
    result->passDetails = details;
    return result;
}

bool CrossImgMTFModel::judgePassOrFail(const QVector<PassDetail> &mtfDataInfo)
{
    for each (PassDetail val in mtfDataInfo)
    {
        if (val.mtfVal > val.upLimit && val.mtfVal < val.lowLimit)
        {
            return false;
        }
    }
    return true;
}
