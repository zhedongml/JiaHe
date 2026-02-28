#include "autoroi.h"
#include "libcbdetect/boards_from_corners.h"
#include "libcbdetect/config.h"
#include "libcbdetect/find_corners.h"
#include "libcbdetect/plot_boards.h"
#include "libcbdetect/plot_corners.h"
int AutoROI::findShortestDis(cv::Point2d pt, std::vector<cv::Point2d> pts, int type)
{
    int mindis = INT16_MAX;
    for (vector<Point2d>::iterator it = pts.begin(); it != pts.end(); ++it) {
        Point2d pt1 = *it;
        int  dis = cal_distance(pt, pt1);
        if (dis < mindis && dis>20) {
            mindis = dis;
        };
    }
    return mindis;
}
int AutoROI::findShortestDis2(cv::Point2d pt, std::vector<ClusterPoint> pts, int type) {
    int mindis = INT16_MAX;
    int minLineDis = -1;
    for (vector<ClusterPoint>::iterator it = pts.begin(); it != pts.end(); ++it) {
        ClusterPoint pt1 = *it;
        int dis = cal_distance(pt, pt1.pt);
        if (dis < mindis && dis>20) {
            mindis = dis;
        }
    }
    return mindis;
}
int AutoROI::findClosestCorner(cv::Point2d centerPt, std::vector<cv::Point2d>& pts) {
    int mindis = INT16_MAX;
    int idx = -1;
    for (vector<Point2d>::iterator it = pts.begin(); it != pts.end(); ++it) {
        Point2d pt1 = *it;
        int  dis = cal_distance(centerPt, pt1);
        if (dis < mindis) {
            mindis = dis;
            idx = it - pts.begin();
        };
    }
    return idx;
}
int AutoROI::findClosestCorner2(cv::Point2d centerPt, std::vector<ClusterPoint>& pts) {
    int mindis = INT16_MAX;
    int minLineDis = -1;
    int idx = -1;
    for (vector<ClusterPoint>::iterator it = pts.begin(); it != pts.end(); ++it) {
        ClusterPoint pt1 = *it;
        int dis = cal_distance(centerPt, pt1.pt);
        if (dis < mindis && dis>20) {
            mindis = dis;
            idx = it - pts.begin();
        }
    }
    return idx;
}
double  AutoROI::cal_distance(cv::Point2d pt1, cv::Point2d pt2) {
    double dy = pow((pt2.y - pt1.y), 2);
    double dx = pow((pt2.x - pt1.x), 2);
    return sqrt(dy + dx);
}
std::vector<ClusterPoint> AutoROI::clusterpoint(std::vector<cv::Point2d> pts, int thresh) {
    std::vector<ClusterPoint> result;
    if (pts.size() > 0) {
        for (int i = 0; i < pts.size(); i++) {
            ClusterPoint clusterPt;
            cv::Point2d pt0 = pts.at(i);
            clusterPt.pt = pt0;
            for (int j = i + 1; j < pts.size(); j++) {
                cv::Point2d pt1 = pts.at(j);
                double dis = cal_distance(pt0, pt1);
                if (dis <= thresh) {
                    clusterPt.pt.x = (clusterPt.pt.x + pt1.x) / 2;
                    clusterPt.pt.y = (clusterPt.pt.y + pt1.y) / 2;
                    clusterPt.count++;
                    pts.erase(pts.begin() + j);
                    j--;
                }
            }
            result.push_back(clusterPt);
        }
    }
    return result;
}
std::vector<cv::Rect> AutoROI::getClosestCenterROI(cv::Mat& img, cv::Point2d pt, AutoROIOption& opt, int xthreshhold, int ythreshhlod) {
    int imgwidth = img.cols;
    int imgheight = img.rows;
    std::vector<cv::Rect> autoRois;
    cv::Point2d roiPt;
    int width = 0;
    int height = 0;
    if (opt.AutoROIDir == 1) {
        if (pt.y >= imgheight / 2) {
            roiPt.x = pt.x - opt.width / 2;
            if (roiPt.x < 0) roiPt.x = 1;
            roiPt.y = pt.y - opt.height - opt.offset;
            if (roiPt.y < 0) roiPt.y = 1;

            width = opt.width > xthreshhold ? xthreshhold : opt.width;
            height = opt.height > ythreshhlod ? ythreshhlod : opt.height;
            if (roiPt.x + width > imgwidth) {
                width = imgwidth - roiPt.x;
            }
            if (roiPt.y + height > imgheight) {
                height = imgheight - roiPt.y;
            }
        }
        else {
            roiPt.x = pt.x - opt.width / 2;
            roiPt.y = pt.y + opt.offset;
            if (roiPt.x < 0) roiPt.x = 1;
            if (roiPt.y < 0) roiPt.y = 1;

            width = opt.width > xthreshhold ? xthreshhold : opt.width;
            height = opt.height > ythreshhlod ? ythreshhlod : opt.height;
            if (roiPt.x + width > imgwidth) {
                width = imgwidth - roiPt.x;
            }
            if (roiPt.y + height > imgheight) {
                height = imgheight - roiPt.y;
            }
        }
    }
    else if (opt.AutoROIDir == 0) {
        if (pt.x >= imgwidth / 2) {
            roiPt.x = pt.x - opt.height - opt.offset;
            roiPt.y = pt.y - opt.width / 2;
            if (roiPt.x < 0) roiPt.x = 1;
            if (roiPt.y < 0) roiPt.y = 1;

            width = opt.width > xthreshhold ? xthreshhold : opt.width;
            height = opt.height > ythreshhlod ? ythreshhlod : opt.height;
            if (roiPt.x + width > imgwidth) {
                width = imgwidth - roiPt.x;
            }
            if (roiPt.y + height > imgheight) {
                height = imgheight - roiPt.y;
            }
        }
        else {
            roiPt.x = pt.x + opt.offset;
            roiPt.y = pt.y - opt.width / 2;
            width = opt.width > xthreshhold ? xthreshhold : opt.width;
            height = opt.height > ythreshhlod ? ythreshhlod : opt.height;
            if (roiPt.x < 0) roiPt.x = 1;
            if (roiPt.y < 0) roiPt.y = 1;
            if (roiPt.x + width > imgwidth) {
                width = imgwidth - roiPt.x;
            }
            if (roiPt.y + height > imgheight) {
                height = imgheight - roiPt.y;
            }
        }
    }
    cv::Rect rect(roiPt.x, roiPt.y, width, height);
    autoRois.push_back(rect);
    return autoRois;
}
cv::Mat AutoROI::convet16bitTo8bit(cv::Mat& rawimg) {
    cv::Mat newImg;
    if (rawimg.depth() == CV_16U)
    {
        double minv = 0.0, maxv = 0.0;
        double* minp = &minv;
        double* maxp = &maxv;
        minMaxIdx(rawimg, minp, maxp);

        if (maxv > 4095.1)
        {
            cv::Mat tmp = rawimg / 65535 * 255;

            tmp.convertTo(newImg, CV_8UC1);
        }
        else
        {
            cv::Mat tmp = rawimg / 4095.0 * 255;

            tmp.convertTo(newImg, CV_8UC1);
        }

        return newImg;
    }
    else {
        return rawimg;
    }

}
cv::Point2d AutoROI::getChroAberratCornnerPoint(cv::Mat cvImg, cv::Point2d nearPt)
{
    cv::Point2d pt(0, 0);
    cv::Mat scaledImg;
    cv::resize(cvImg, scaledImg, cv::Size(scaled * cvImg.cols, scaled * cvImg.rows));

    Harris h;
    cv::blur(scaledImg, scaledImg, cv::Size(15, 15));
    cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(30, 30));
    cv::morphologyEx(scaledImg, scaledImg, cv::MORPH_CLOSE, element1);
    cv::GaussianBlur(scaledImg, scaledImg, cv::Size(15, 15), 0, 0);
    vector<Point2d> resPts = h.detect_corners(scaledImg, 25, 3.414);
    std::vector<ClusterPoint> clusterPts = clusterpoint(resPts);
    cv::Mat resultimg = convet16bitTo8bit(scaledImg);
    for (vector<ClusterPoint>::iterator it = clusterPts.begin(); it != clusterPts.end(); ++it) {

        ClusterPoint center = (*it);
        cv::circle(resultimg, center.pt, 3, cv::Scalar(0, 0, 255), -1);
    }
    int imgwidth = cvImg.cols;
    int imgheight = cvImg.rows;
    cv::Point2d centerPt;
    if (nearPt.x == 0 && nearPt.y == 0) {
        centerPt.x = imgwidth * scaled / 2;
        centerPt.y = imgheight * scaled / 2;
    }
    else
    {
        centerPt.x = nearPt.x * scaled;
        centerPt.y = nearPt.y * scaled;
    }
    int index = findClosestCornerIdx(centerPt, clusterPts);
    pt = clusterPts.at(index).pt;
    cv::Point2d center = pt / scaled;

    cv::Rect rectfull(0, 0, cvImg.cols, cvImg.rows);
    cv::Rect rect(center.x - roiwidth / 2, center.y - roiheight / 2, roiwidth, roiheight);
    rect = rect & rectfull;
    cv::Mat roiimg = cvImg(rect);
    algorithm::MLCrossCenter my_cross;
    cv::Point2f crosscenter = my_cross.GetCrossCenter(roiimg);
    center.x = int(crosscenter.x + 0.5) + rect.x;
    center.y = int(crosscenter.y + 0.5) + rect.y;
    return center;
}
ChromaticAberration* AutoROI::autoChromaticAberration(cv::Mat img1, cv::Mat img2, cv::Mat img3)
{
    ChromaticAberration* result = new ChromaticAberration[3];
    cv::Point2d basePt;
    basePt = getChroAberratCornnerPoint(img1);
    ChromaticAberration ca1;
    ca1.id = 1;
    ca1.pointx = basePt.x;
    ca1.pointy = basePt.y;
    ca1.diffx = 0;
    ca1.diffy = 0;
    result[0]= ca1;
    cv::Point2d center2;
    center2 = getChroAberratCornnerPoint(img2,basePt);
    ChromaticAberration ca2;
    ca2.id = 2;
    ca2.pointx = center2.x;
    ca2.pointy = center2.y;
    ca2.diffx = center2.x - basePt.x;
    ca2.diffy = center2.y - basePt.y;
    result[1] = ca2 ;
    cv::Point2d center3;
    center3 = getChroAberratCornnerPoint(img3, basePt);
    ChromaticAberration ca3;
    ca3.id = 3;
    ca3.pointx = center3.x;
    ca3.pointy = center3.y;
    ca3.diffx = center3.x - basePt.x;
    ca3.diffy = center3.y - basePt.y;
    result[2] = ca3;
    return result;
}
int AutoROI::findClosestCornerIdx(cv::Point2d centerPt, std::vector<ClusterPoint>& pts)
{
    int mindis = INT16_MAX;
    int minLineDis = -1;
    int idx = -1;
    for (vector<ClusterPoint>::iterator it = pts.begin(); it != pts.end(); ++it) {
        ClusterPoint pt1 = *it;
        int dis = cal_distance(centerPt, pt1.pt);
        if (dis < mindis) {
            mindis = dis;
            idx = it - pts.begin();
        }
    }
    return idx;
}
std::vector<cv::Rect> AutoROI::getAutoROIs(cv::Mat& img, cv::Point2d pt, AutoROIOption& opt, int xthreshhold, int ythreshhlod) {
    int imgwidth = img.cols;
    int imgheight = img.rows;
    std::vector<cv::Rect> autoRois;
    for (int j = 0; j < opt.count; j++) {
        if (opt.AutoROIDir == 0)
            j = 3;
        if (j == 0) {
            cv::Point2d roiPt;
            roiPt.x = pt.x - opt.width / 2;
            if (roiPt.x < 0) roiPt.x = 1;
            roiPt.y = pt.y - opt.height - opt.offset;
            if (roiPt.y < 0) roiPt.y = 1;

            int width = opt.width > xthreshhold ? xthreshhold : opt.width;
            int height = opt.height > ythreshhlod ? ythreshhlod : opt.height;
            if (roiPt.x + width > imgwidth) {
                width = imgwidth - roiPt.x;
            }
            if (roiPt.y + height > imgheight) {
                height = imgheight - roiPt.y;
            }
            cv::Rect rect(roiPt.x, roiPt.y, width, height);
            autoRois.push_back(rect);

        }
        else if (j == 1) {
            cv::Point2d roiPt;
            roiPt.x = pt.x - opt.width / 2;
            roiPt.y = pt.y + opt.offset;
            if (roiPt.x < 0) roiPt.x = 1;
            if (roiPt.y < 0) roiPt.y = 1;

            int width = opt.width > xthreshhold ? xthreshhold : opt.width;
            int height = opt.height > ythreshhlod ? ythreshhlod : opt.height;
            if (roiPt.x + width > imgwidth) {
                width = imgwidth - roiPt.x;
            }
            if (roiPt.y + height > imgheight) {
                height = imgheight - roiPt.y;
            }
            cv::Rect rect(roiPt.x, roiPt.y, width, height);
            autoRois.push_back(rect);
        }
        else if (j == 2) {
            cv::Point2d roiPt;
            roiPt.x = pt.x - opt.height - opt.offset;
            roiPt.y = pt.y - opt.width / 2;
            if (roiPt.x < 0) roiPt.x = 1;
            if (roiPt.y < 0) roiPt.y = 1;

            int width = opt.width > xthreshhold ? xthreshhold : opt.width;
            int height = opt.height > ythreshhlod ? ythreshhlod : opt.height;
            if (roiPt.x + width > imgwidth) {
                width = imgwidth - roiPt.x;
            }
            if (roiPt.y + height > imgheight) {
                height = imgheight - roiPt.y;
            }
            cv::Rect rect(roiPt.x, roiPt.y, height, width);
            autoRois.push_back(rect);
        }
        else if (j == 3) {
            cv::Point2d roiPt;
            roiPt.x = pt.x + opt.offset;
            roiPt.y = pt.y - opt.width / 2;
            int width = opt.width > xthreshhold ? xthreshhold : opt.width;
            int height = opt.height > ythreshhlod ? ythreshhlod : opt.height;
            if (roiPt.x < 0) roiPt.x = 1;
            if (roiPt.y < 0) roiPt.y = 1;
            if (roiPt.x + width > imgwidth) {
                width = imgwidth - roiPt.x;
            }
            if (roiPt.y + height > imgheight) {
                height = imgheight - roiPt.y;
            }
            cv::Rect rect(roiPt.x, roiPt.y, height, width);
            autoRois.push_back(rect);
        }
    }
    return autoRois;
}
std::vector<cv::Rect> AutoROI::crosscenterAutoROI(cv::Mat& img,AutoROIOption& opt) {
    int imgwidth = img.cols;
    int imgheight = img.rows;
    algorithm::MLCrossCenter my_cross;
    std::vector<cv::Rect> autoRois;
    cv::Point2f center = my_cross.GetCrossCenter(img);
    std::vector<cv::Point2f> rois_center = my_cross.GetROICenter(opt.offset);
 
    cv::Point2d roiPt;
    cv::Rect imgrect(0, 0, imgwidth, imgheight);
    for (int i = 0; i < rois_center.size(); i++)
    { 
        roiPt = rois_center.at(i);
        cv::Rect rect(roiPt.x - opt.width/2, roiPt.y - opt.height/2, opt.width, opt.height);
        autoRois.push_back(rect & imgrect);
    }
   
    return autoRois;
}
std::vector<cv::Rect> AutoROI::findAutoROIs(cv::Mat img, int type, int count, int roiwidth, int roiheight, int offset, bool isThroFocus, int roiDir) {
    AutoROIOption roiOpt;
    roiOpt.type = type;
    roiOpt.count = count;
    roiOpt.offset = offset;
    roiOpt.width = roiwidth;
    roiOpt.height = roiheight;
    roiOpt.applytoallImg = isThroFocus;
    roiOpt.AutoROIDir = roiDir;
    roiOpt.closeCenter = true;
    switch (roiOpt.type)
    {
        case (1):
        case (3):
           return chessboardAutoROI(img,roiOpt);
            break;
        case (2):
          return  crosscenterAutoROI(img,roiOpt);
        case (4):
            return dashlinechessboardAutoROI(img, roiOpt);
        default:
            break;
    }
}
std::vector<cv::Rect> AutoROI::dashlinechessboardAutoROI(cv::Mat& img, AutoROIOption& opt)
{
    double scaled = 0.5;
    int imgwidth = img.cols;
    int imgheight = img.rows;
    if (opt.width > 0 && opt.height > 0) {
        int xthresh = -1, ythresh = -1;
        cv::Mat roiimg;
        cv::resize(img, roiimg, cv::Size(scaled * img.cols, scaled * img.rows));
        try {
            if (roiimg.data) {
                int offset = opt.offset;
                //cv::Mat resultimg;
               // cv::cvtColor(roiimg, resultimg, cv::COLOR_GRAY2BGR);
                cv::Point2d pt(0, 0);

                {
                    Harris h;
                    cv::blur(roiimg, roiimg, cv::Size(15, 15));
                    cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(30, 30));
                    cv::morphologyEx(roiimg, roiimg, cv::MORPH_CLOSE, element1);
                    cv::GaussianBlur(roiimg, roiimg, cv::Size(15, 15), 0, 0);
                    vector<Point2d> resPts = h.detect_corners(roiimg, 25, 3.414);

                    std::vector<ClusterPoint> clusterPts = clusterpoint(resPts);
                    /*for (vector<ClusterPoint>::iterator it = clusterPts.begin(); it != clusterPts.end(); ++it) {
                        ClusterPoint center = (*it);
                        cv::circle(resultimg, center.pt, 3, cv::Scalar(0, 0, 255), -1);
                    }*/
                    if (opt.closeCenter) {
                        cv::Point2d centerPt(imgwidth * scaled / 2, imgheight * scaled / 2);
                        int index = findClosestCorner2(centerPt, clusterPts);
                        pt = clusterPts.at(index).pt;
                    }
                    else {
                        int rndidx = rand() % clusterPts.size();
                        pt = clusterPts.at(rndidx).pt;
                    }
                    xthresh = findShortestDis2(pt, clusterPts, 0) * (1.0 / scaled);
                    ythresh = findShortestDis2(pt, clusterPts, 1) * (1.0 / scaled);
                }
                if (pt.x != 0 || pt.y != 0) {
                    pt.x = pt.x * (1.0 / scaled);
                    pt.y = pt.y * (1.0 / scaled);
                    if (opt.closeCenter) {
                       return getClosestCenterROI(img,pt,opt,xthresh, ythresh);
                    }
                    else {
                        if (opt.applytoallImg) {
                            opt.count = 1;
                            return getAutoROIs(img, pt, opt, xthresh, ythresh);
                        }
                        else
                            return getAutoROIs(img, pt, opt, xthresh, ythresh);
                    }
                }
                //cv::imshow("test", resultimg);
                //cv::waitKey(0);
            }
        }
        catch (...) {
            std::vector<cv::Rect> result;
            return result;
        }
    }
}
std::vector<cv::Rect> AutoROI::chessboardAutoROI(cv::Mat& img, AutoROIOption& opt)
{
    cbdetect::Corner corners;
    cbdetect::Params params;
    params.corner_type = cbdetect::SaddlePoint;
    params.show_debug_image = false;
    params.find_corners_reiszed = false;
    int imgwidth = img.cols;
    int imgheight = img.rows;
    if (opt.width > 0 && opt.height > 0) {
        int xthresh = -1, ythresh = -1;
        int tempwidth = 8 * (opt.width);
        int tempheight = 8 * (opt.height);
        int roiWidth = tempwidth > imgwidth ? imgwidth : tempwidth;
        int roiHeight = tempheight > imgheight ? imgheight : tempheight;
        cv::Rect rect(imgwidth / 2 - roiWidth / 2, imgheight / 2 - roiHeight / 2, roiWidth, roiHeight);
        cv::Mat roiimg = img(rect);
        try {
            if (roiimg.data) {
                int offset = opt.offset;
                cv::Mat resultimg;
                cv::cvtColor(roiimg, resultimg, cv::COLOR_GRAY2BGR);
                cv::Point2d pt(0, 0);
                if (opt.type == 1)
                    cbdetect::find_corners(roiimg, corners, params);
                if (corners.p.size() > 1) {
                    //cbdetect::plot_corners(resultimg, corners);
                    if (opt.closeCenter) {
                        cv::Point2d centerPt(roiWidth / 2, roiHeight / 2);
                        int index = findClosestCorner(centerPt, corners.p);
                        pt = corners.p.at(index);
                    }
                    else {
                        int rndidx = rand() % corners.p.size();
                        pt = corners.p.at(rndidx);
                    }
                    xthresh = findShortestDis(pt, corners.p, 0);
                    ythresh = findShortestDis(pt, corners.p, 1);
                }
                else {
                    Harris h;
                    vector<Point2d> resPts = h.detect_corners(roiimg, 25, 3.414);
                    std::vector<ClusterPoint> clusterPts = clusterpoint(resPts);
                    for (vector<ClusterPoint>::iterator it = clusterPts.begin(); it != clusterPts.end(); ++it) {
                        ClusterPoint center = (*it);
                        cv::circle(resultimg, center.pt, 3, cv::Scalar(0, 0, 255), -1);
                    }
                    /*if (resultimg.depth() == CV_16U)
                    {
                        Mat outtmp;// = Mat(roi.cols, roi.rows, CV_8UC1);

                        double minv = 0.0, maxv = 0.0;
                        double* minp = &minv;
                        double* maxp = &maxv;
                        minMaxIdx(resultimg, minp, maxp);

                        if (maxv > 4095.1)
                        {
                            cv::Mat tmp = resultimg / 65535 * 255;

                            tmp.convertTo(resultimg, CV_8UC1);
                        }
                        else
                        {
                            cv::Mat tmp = resultimg / 4095.0 * 255;

                            tmp.convertTo(resultimg, CV_8UC1);
                        }

                    }*/
                    if (opt.closeCenter) {
                        cv::Point2d centerPt(roiWidth / 2, roiHeight / 2);
                        int index = findClosestCorner2(centerPt, clusterPts);
                        pt = clusterPts.at(index).pt;
                    }
                    else {
                        int rndidx = rand() % clusterPts.size();
                        pt = clusterPts.at(rndidx).pt;
                    }
                    xthresh = findShortestDis2(pt, clusterPts, 0);
                    ythresh = findShortestDis2(pt, clusterPts, 1);
                }
                if (pt.x != 0 || pt.y != 0) {
                    pt.x = pt.x + rect.x;
                    pt.y = pt.y + rect.y;
                    if (opt.closeCenter) {
                        return getClosestCenterROI(img, pt, opt, xthresh, ythresh);
                    }
                    else {
                        if (opt.applytoallImg) {
                            opt.count = 1;
                            return getAutoROIs(img, pt, opt, xthresh, ythresh);
                        }
                        else
                            return  getAutoROIs(img, pt, opt, xthresh, ythresh);
                    }
                }
                //cv::imshow("test", resultimg);
                //cv::waitKey(0);
            }
        }
        catch (...) {
            std::vector<cv::Rect> result;
            return result;
        }
    }
}