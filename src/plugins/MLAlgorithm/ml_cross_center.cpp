#include "ml_cross_center.h"


//using namespace cv;
//using namespace std;
using namespace algorithm;

MLCrossCenter::MLCrossCenter()
{
}

MLCrossCenter::~MLCrossCenter()
{
}

//MLCrossCenter::MLCrossCenter(Mat img, int off, int size)
//{
//    this->img_ = img.clone();
//    this->size_ = size;
//    this->off_ = off;
//    //this->roiLen = roiLen;
//    
//
//}

cv::Point2f MLCrossCenter::GetCrossCenter(cv::Mat img, int size)
{
    this->img_ = img.clone();
    this->size_ = size;
    if (FindCenter())
    {
        float std1, std2;
        if (*max_element(my_angle1_.begin(), my_angle1_.end()) - *min_element(my_angle1_.begin(), my_angle1_.end()) > 3)
            for_each(begin(my_angle1_), end(my_angle1_), [&](float& x) {x = fabs(x); });
        if (*max_element(my_angle2_.begin(), my_angle2_.end()) - *min_element(my_angle2_.begin(), my_angle2_.end()) > 3)
            for_each(begin(my_angle2_), end(my_angle2_), [&](float& x) {x = fabs(x); });
        std1 = STDVector(my_angle1_);
        std2 = STDVector(my_angle2_);
        
        if ((std1 > 0.01 || std2 > 0.01) && line_width_!=0)
        {
            my_angle1_.clear();
            my_angle2_.clear();
            int roi_size = 10 * line_width_;
            center_offset_ = center_ - cv::Point2f(roi_size / 2, roi_size / 2);
            center_offset_.x = center_offset_.x < 0 ? 0 : center_offset_.x;
            center_offset_.y = center_offset_.y < 0 ? 0 : center_offset_.y;
            cv::Rect ROI = cv::Rect(center_offset_, cv::Size(roi_size, roi_size));
            cv::Rect full_roi = cv::Rect(0, 0, img_cols_, img_rows_);
            
            ROI = ROI & full_roi;
            if (ROI.width < size || ROI.height < size)
                ROI = full_roi;
            this->img_ = this->img_(ROI);

            FindCenter();
        }

        return center_ + center_offset_;
    }

    else
        return cv::Point2f(0, 0);
}

void MLCrossCenter::SeparateLines(std::vector<cv::Vec4i>& lines, std::vector<cv::Vec4i>& line1, std::vector<float>& ks1, std::vector<cv::Vec4i>& line2, std::vector<float>& ks2)
{
    
    
    float k1 = float((lines[0][3] - lines[0][1])) / float((lines[0][2] - lines[0][0]));
    float err = 20;
    double angle1, angle2;


    line1.push_back(lines[0]);            
    ks1.push_back(k1);
    //k1 = -INFINITY;
    angle1 = (atan(k1));
    my_angle1_.push_back(angle1);
    for (int i = 1; i < lines.size(); i++)
    {
        float k2 = float((lines[i][3] - lines[i][1])) / float((lines[i][2] - lines[i][0]));
        angle2 = (atan(k2));
        //if ((k1 == 0 && (k2 == INFINITY || k2 == -INFINITY || k2>tan(90 - err) || k2<tan(90 + err)))||((k1 == INFINITY|| k1 == -INFINITY) && (k2 == 0 || (k2>tan(-err) && k2<tan(err))))||(k1*k2 <= -0.3 && k1*k2 >= -1.3))
        if (fabs(angle1 - angle2)<err / 180 * CV_PI || fabs(angle1 - angle2)>(180 - err / 2) / 180 * CV_PI)
        {
            ks1.push_back(k2);
            line1.push_back(lines[i]);
            my_angle1_.push_back(angle2);
        }
        else
        {

            ks2.push_back(k2);
            line2.push_back(lines[i]);
            my_angle2_.push_back(angle2);
        }
    }
}

cv::Point2f algorithm::MLCrossCenter::CalculateCross(std::vector<cv::Vec4i> lines, int &width1, int &width2)
{   
        // algorithm::MLCrossCenter my_cross;
        std::vector<cv::Vec4i> lines1, lines2;
        std::vector<float> ks1, ks2;
        float a1, b1, c1, a2, b2, c2;
        double line_width;
        cv::Point2f center_cross;// = { 0,0 };
        if (!lines.empty())
        {
            SeparateLines(lines, lines1, ks1, lines2, ks2);
            if (!lines1.empty() & !lines2.empty())
            {
                // int width1, width2;
                width1 = CalculateLine(lines1, ks1, a1, b1, c1);
                width2 = CalculateLine(lines2, ks2, a2, b2, c2);
                line_width = std::max(width1, width2);
                if (width1 <= 20 & width2 <= 20)
                {
                    center_cross = CalculateLinesCross(a1, b1, c1, a2, b2, c2);
                }
            }
        }
        return center_cross;
    


}

int MLCrossCenter::CalculateLine(std::vector<cv::Vec4i>& lines, std::vector<float>& ks, float& a, float& b, float& c)
{
    int width;
    bool is_inf;
    float ksum = 0, asum = 0, bsum = 0, csum = 0;
    //float kk, bb;
    std::vector<float>::iterator ret1,ret2;


    ret1 = std::find(ks.begin(), ks.end(), INFINITY);
    ret2 = std::find(ks.begin(), ks.end(), -INFINITY);
    if (ret1 != ks.end() || ret2 != ks.end())
    {
        ksum = INFINITY;
        //is_inf = true;
    }
    else
    {
        ksum = std::accumulate(ks.begin(), ks.end(), 0.0);
        //is_inf = false;
    }

    ksum = ksum / lines.size();

    std::vector<float> A, B, C;
    if (abs(ksum) >1 )
    {
        a = 1;
        float bb, cc;
        for (int i = 0; i < lines.size(); i++)
        {
            bb = -float((lines[i][2] - lines[i][0])) / float((lines[i][3] - lines[i][1]));
            B.push_back(bb);
            bsum += bb;
            //csum += cc;
        }
        b = bsum / lines.size();
        //b = (*max_element(B.begin(), B.end()) + *min_element(B.begin(), B.end())) / 2;
        for (int i = 0; i < lines.size(); i++)
        {
            cc = -(float(lines[i][0] + float(b*lines[i][1])) + float(lines[i][2] + float(b*lines[i][3])))/2;
            C.push_back(cc);
            //csum += cc;
        }


        ReduceError(C);
        c = (*max_element(C.begin(), C.end()) + *min_element(C.begin(), C.end())) / 2;

        //c = -838.6;
        //c = csum / lines.size();
    }
    else
    {
        b = 1;
        float aa, cc;
        for (int i = 0; i < lines.size(); i++)
        {
            aa = -float((lines[i][3] - lines[i][1])) / float((lines[i][2] - lines[i][0]));
            A.push_back(aa);
            asum += aa;
            //csum += cc;
        }
        a = asum / lines.size();
        //a = (*max_element(A.begin(), A.end()) + *min_element(A.begin(), A.end())) / 2;
        for (int i = 0; i < lines.size(); i++)
        {
            cc = -(float(lines[i][1] + float(a*lines[i][0])) + float(lines[i][3] + float(a*lines[i][2])))/2;
            C.push_back(cc);
            //csum += cc;
        }
        ReduceError(C);
        c = (*max_element(C.begin(), C.end()) + *min_element(C.begin(), C.end())) / 2;

        
        //c = csum / lines.size();
    }

    width = ceil(*max_element(C.begin(), C.end()) - *min_element(C.begin(), C.end()));
    return width;

}


cv::Point2f MLCrossCenter::CalculateLinesCross(float a1, float b1, float c1, float a2, float b2, float c2)
{
    cv::Point2f cross;

    cross.x = -(c1*b2 - c2*b1) / (a1*b2 - a2*b1);
    cross.y = -(c1*a2 - c2*a1) / (b1*a2 - b2*a1);

    return cross;
}




bool MLCrossCenter::FindCenter()
{
    cv::Mat img_gray;
    if (this->img_.channels() == 3)//转灰度图
        cvtColor(this->img_, img_gray, cv::COLOR_BGR2GRAY);
    else
        img_gray = this->img_.clone();
    cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(size_, size_));
    cv::Mat element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 25));
    //Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight));
    img_rows_ = img_.rows;
    img_cols_ = img_.cols;

    //cout << img_gray.type() << endl;
    img_gray.convertTo(img_gray, CV_8UC1);

    cv::Mat img_draw = cv::Mat::zeros(img_gray.size(),CV_8UC3);
    //cv::GaussianBlur(img_gray, img_gray, cv::Size(3, 3), 0.001);
    cv::medianBlur(img_gray, img_gray, 3);
    cv::morphologyEx(img_gray, img_gray, cv::MORPH_CLOSE, element1);
    double minv = 0.0, maxv = 0.0;
    cv::Point minp, maxp;
    minMaxLoc(img_gray, &minv, &maxv, &minp, &maxp);

    
    //cv::floodFill(img_gray, cv::Point(0, 0), 0, 0, 5, 5);
    threshold(img_gray, img_gray, 0, 255, cv::THRESH_OTSU);
    //threshold(img_gray, img_gray, (minv + maxv) / 2, 255, cv::THRESH_BINARY);
    //morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
    //adaptiveThreshold(img_gray, img_gray, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY,3,1);
    std::vector<cv::Vec4i> lines;
    std::vector<cv::Vec4i> lines1, lines2;
    
    //img_gray = 255 - img_gray;
    cv::Mat m;
    img_gray.convertTo(m, CV_64FC1);
    cv::Mat m_row(1, img_cols_, CV_64FC1, cv::Scalar(0));
    cv::Mat m_col(img_rows_, 1, CV_64FC1, cv::Scalar(0));


    cv::reduce(m, m_row, 0, cv::REDUCE_SUM);
    cv::reduce(m, m_col, 1, cv::REDUCE_SUM);

    int numR = countNonZero(m_row);
    int numC = countNonZero(m_col);

    int l = (numR < numC ? numR : numC)/9;



    cv::Canny(img_gray, img_gray, 125, 300);
    cv::HoughLinesP(img_gray, lines, 1, CV_PI / 180, l, l, l);


    for (int i = 0; i < lines.size(); i++)
    {
    	line(img_draw, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), cv::Scalar(rand() % 255, rand() % 255, rand() % 255),3);
    }

    std::vector<float> ks1, ks2;

    if (!lines.empty())
    {
        SeparateLines(lines, lines1, ks1, lines2, ks2);
        

        if (!lines1.empty() && !lines2.empty())
        {
            int width1, width2;
            width1 = CalculateLine(lines1, ks1, a1_, b1_, c1_);
            width2 = CalculateLine(lines2, ks2, a2_, b2_, c2_);
            
            line_width_ = std::max(width1, width2);
            //calFitLine(lines1, ks1, a1, b1, c1);
            //calFitLine(lines2, ks2, a2, b2, c2);
            //calFitPoly(lines1, ks1, a1, b1, c1);
            //calFitPoly(lines2, ks2, a2, b2, c2);

            //for (int i = 0; i < lines1.size(); i++)
            //{
            //    line(img_draw, Point(lines1[i][0], lines1[i][1]), Point(lines1[i][2], lines1[i][3]), Scalar(0, 0, 255),1);
            //}

            //for (int i = 0; i < lines2.size(); i++)
            //{
            //    line(img_draw, Point(lines2[i][0], lines2[i][1]), Point(lines2[i][2], lines2[i][3]), Scalar(0, 255, 0),1);
            //}
            
            center_ = CalculateLinesCross(a1_, b1_, c1_, a2_, b2_, c2_);
            //center = calPolyCross(a1, b1, c1, a2, b2, c2);
            //circle(img_draw, center_, 1, Scalar(255, 255, 255), -1);

            return true;
        }
        else
            return false;

    }
    else
        return false;
}




std::vector<cv::Point2f> MLCrossCenter::FindROICenter(cv::Point2f center, float a1, float b1, float c1, float a2, float b2, float c2)
{
    std::vector<cv::Point2f> rcenter;
    if (/*FindCenter()*/center_.x != 0 || center_.y != 0)
    {



        float k1 = fabs(a1 / b1);
        float k2 = fabs(a2 / b2);


        float kh = k1 < k2 ? a1 / b1 : a2 / b2;
        float kv = k1 > k2 ? a1 / b1 : a2 / b2;


        float anglev = atan(kv);
        float angleh = atan(kh);

        if (anglev > 0)
        {
            rcenter.push_back(cv::Point2f(center.x + cos(anglev) * off_, center.y - sin(anglev) * off_));
            rcenter.push_back(cv::Point2f(center.x - cos(anglev) * off_, center.y + sin(anglev) * off_));
        }
        else
        {
            rcenter.push_back(cv::Point2f(center.x - cos(anglev) * off_, center.y + sin(anglev) * off_));
            rcenter.push_back(cv::Point2f(center.x + cos(anglev) * off_, center.y - sin(anglev) * off_));
        }

        rcenter.push_back(cv::Point2f(center.x - cos(angleh) * off_, center.y + sin(angleh) * off_));
        rcenter.push_back(cv::Point2f(center.x + cos(angleh) * off_, center.y - sin(angleh) * off_));


    }

    return rcenter;
}






float MLCrossCenter::STDVector(std::vector<float> resultSet)
{
    float sum = std::accumulate(std::begin(resultSet), std::end(resultSet), 0.0);
    float mean = sum / resultSet.size(); //均值

    float accum = 0.0;
    std::for_each(std::begin(resultSet), std::end(resultSet), [&](const double d) {
        accum += (d - mean) * (d - mean);
        });

    float stdev = sqrt(accum / (resultSet.size() - 1)); //方差
    return stdev;
}



std::vector<cv::Point2f> MLCrossCenter::GetROICenter(int off)
{
    this->off_ = off;
    roi_center_ = FindROICenter(center_ + center_offset_, a1_, b1_, c1_, a2_, b2_, c2_);
    return roi_center_;
}

int MLCrossCenter::GetLineWidth()
{
    return line_width_;
}


void MLCrossCenter::ReduceError(std::vector<float>& x)
{
    float sum = std::accumulate(std::begin(x), std::end(x), 0.0);
    float mean = sum / x.size(); //均值
    float std = STDVector(x);
    std::vector<float>::iterator x_it;
    for (x_it = x.begin(); x_it != x.end();)
    {
        if (abs(*x_it - mean) > 2 * std)
        {
            x_it = x.erase(x_it);
        }
        else
        {
            x_it++;
        }
    }
}