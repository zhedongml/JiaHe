#include "DashLineAutoRoi.h"
DashLineAutoRoi::DashLineAutoRoi() {

}
cv::Mat DashLineAutoRoi::convet16bitTo8bit(cv::Mat& rawimg) {
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
cv::Point2d DashLineAutoRoi::getCornnerPoint(cv::Mat cvImg, cv::Point2d nearPt)
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
        centerPt.x =  imgwidth * scaled / 2;
        centerPt.y =  imgheight * scaled / 2;
    }
    else
    {
        centerPt.x = nearPt.x*scaled;
        centerPt.y = nearPt.y*scaled;
    }
    int index = findClosestCorner2(centerPt, clusterPts);
    pt = clusterPts.at(index).pt; 
    return pt/scaled;
}
double  DashLineAutoRoi::cal_distance(cv::Point2d pt1, cv::Point2d pt2) {
    double dy = pow((pt2.y - pt1.y), 2);
    double dx = pow((pt2.x - pt1.x), 2);
    return sqrt(dy + dx);
}
std::vector<DashLineAutoRoi::ClusterPoint> DashLineAutoRoi::clusterpoint(std::vector<cv::Point2d> pts, int thresh) {
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
int DashLineAutoRoi::findClosestCorner2(cv::Point2d centerPt, std::vector<ClusterPoint>& pts) {
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