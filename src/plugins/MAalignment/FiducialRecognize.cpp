#include "FiducialRecognize.h"
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include "piMotionActor/FiducialConfig.h"

FiducialRecognize *FiducialRecognize::m_instance = nullptr;

FiducialRecognize::FiducialRecognize()
{
}
FiducialRecognize::~FiducialRecognize()
{
}

FiducialRecognize *FiducialRecognize::instance()
{
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new FiducialRecognize();
        }
    }
    return m_instance;
}

Result FiducialRecognize::circleDetect(cv::Mat src, Rect rect, int circleRadius, cv::Point2f &center, int circleRadiusMax)
{
    clock_t t1 = clock();

    Mat grayImage;
    if (src.channels() == 4)
    {
        cvtColor(src, grayImage, COLOR_BGRA2GRAY);
    }
    else if (src.channels() == 3)
        cvtColor(src, grayImage, COLOR_BGR2GRAY);
    else
        grayImage = src.clone();
    grayImage = grayImage(rect);

    int startTime = QDateTime::currentMSecsSinceEpoch();
    Mat filterImg;

    if(0){
        // 1. 直方图均衡化
        Mat histEq;
        equalizeHist(grayImage, histEq);

        // 2. 线性对比度增强（alpha:对比度, beta:亮度偏移）
        Mat contrast;
        grayImage.convertTo(contrast, -1, 1.5, 10); // alpha=1.5, beta=10

        // 3. 自适应直方图均衡化（CLAHE）
        Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
        Mat claheImg;
        clahe->apply(grayImage, claheImg);

        // 4. 锐化（增强边缘）
        Mat sharp;
        Mat kernel = (Mat_<float>(3, 3) <<
            0, -1, 0,
            -1, 5, -1,
            0, -1, 0);
        filter2D(grayImage, sharp, grayImage.depth(), kernel);
    }

    {
        Mat enhanced;
        GaussianBlur(grayImage, enhanced, Size(3, 3), 0);
        Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
        clahe->apply(enhanced, enhanced);
        Mat sharpened;
        Mat sharpKernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
        filter2D(enhanced, sharpened, enhanced.depth(), sharpKernel);

        filterImg = sharpened;
        //myfilter2(grayImage, filterImg);
    }

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;

    Mat blurImage;
    GaussianBlur(filterImg, blurImage, Size(3, 3), 1.0);

    Mat edgesImage;
    //Canny(blurImage, edgesImage, 1, 100);
    //cv::dilate(edgesImage, edgesImage, cv::Mat(), Point(-1, -1), 1);

    startTime = QDateTime::currentMSecsSinceEpoch();

    vector<Vec3f> circles;
    int minDist = 30;
    int param1 = 100;
    int param2 = 45;
    int minRadius = circleRadius - 10;
    int maxRadius = circleRadiusMax == 0 ? circleRadius + 10 : circleRadiusMax;
    HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);

    if(circles.size() == 0){
        circles.clear();
        param2 = param2 - 5;
        HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);
    }
    else if (circles.size() > 1) {
        circles.clear();
        param2 = param2 + 10;
        HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);

        if (circles.size() > 1)
        {
            circles.clear();
            param2 = param2 + 10;
            HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);
        }
    }

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    //qWarning() << "---- HoughCircles ====== " << takeTime << " ms...";

    cv::Mat color;
    cv::cvtColor(src, m_img_draw, cv::COLOR_GRAY2RGB);
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point centerTmp(cvRound(circles[i][0]), cvRound(circles[i][1]));
        center = centerTmp + Point(rect.x, rect.y);
        circle(m_img_draw, center, 2, Scalar(0, 0, 255), -1, 8, 0);

        int radius = cvRound(circles[i][2]);
        circle(m_img_draw, center, radius, Scalar(0, 0, 255), 1, 8, 0);
    }

    //static int mm = 1;
    //cv::imwrite(QString("D:/00/img_%1.tiff").arg(mm++).toStdString(), m_img_draw);

     clock_t t3 = clock();
     double timeForDrawCircle = (double)(t3 - t1) / CLOCKS_PER_SEC * 1000;
     //cout << "circles size: " << circles.size() << ". " << "HoughCircles:"
     //<< timeForDrawCircle << "ms" << endl;

    if (circles.size() != 1)
    {
        return Result(false,
                      QString("Detect circle error, circle size is %1.").arg(circles.size()).toStdString(),
                      circles.size() && param2 > 35);
    }
    return Result();
}

Result FiducialRecognize::circleRadius(string fileName, float &radius, Rect &rect)
{
    clock_t t1 = clock();

    Mat grayImage, src;
    src = imread(fileName, IMREAD_UNCHANGED);
    if (src.empty())
    {
        return Result(false, "Detect fiducial circle templete error, image is empty.");
    }

    if (src.channels() == 4)
    {
        cvtColor(src, grayImage, COLOR_BGRA2GRAY);
    }
    else if (src.channels() == 3)
        cvtColor(src, grayImage, COLOR_BGR2GRAY);
    else
        grayImage = src.clone();

    Mat filterImg;
    myfilter2(grayImage, filterImg);

    Mat blurImage;
    GaussianBlur(filterImg, blurImage, Size(9, 9), 2, 2);

    Mat edgesImage;
    //Canny(blurImage, edgesImage, 1, 10);

    vector<Vec3f> circles;
    int minDist = 30;   
    int param1 = 10;    
    int param2 = 35;    
    int minRadius = 10; 
    int maxRadius = src.cols/2 + 5;

    int min = src.rows > src.cols ? src.cols : src.rows;
    if (min / 2 < maxRadius)
    {
        maxRadius = min / 2;
    }

    HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);
    for(int i = 0; i < 3; ++i){
        if (circles.size() == 1)
        {
            break;
        }
        param2 += 10;
        HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);
    }
    
    clock_t t2 = clock();
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        radius = cvRound(circles[i][2]);
        circle(src, center, 3, Scalar(0, 0, 255), -1, 8, 0);    
    }
    //clock_t t3 = clock();
    //double timeForHoughCircle = (double)(t2 - t1) / CLOCKS_PER_SEC * 1000;
    //double timeForDrawCircle = (double)(t3 - t2) / CLOCKS_PER_SEC * 1000;
    //cout << "CircleRadius:" << timeForHoughCircle << "ms, DrawCircle:" << timeForDrawCircle << "ms" << endl;

    if (circles.size() != 1)
    {
        return Result(false, QString("Detect circle templete radius error, circle size is %1.").arg(circles.size()).toStdString());
    }

    rect.x = cvRound(circles[0][0]);
    rect.y = cvRound(circles[0][1]);
    rect.width = radius * 2;
    rect.height = radius * 2;
    return Result();
}

Result FiducialRecognize::getTmplRadius(std::string fiducialType, int fiducialNo, float &radius)
{
    QString fiducialTypeQ = QString::fromStdString(fiducialType);
    if (m_radiusMap.contains(fiducialTypeQ) && m_radiusMap[fiducialTypeQ].contains(fiducialNo)){
        radius = m_radiusMap[fiducialTypeQ][fiducialNo];
        return Result();
    }

    std::string imageFile = FiducialConfig::instance()->getTempleteFilePath(fiducialType, fiducialNo);

    Rect rectTmp;
    Result ret = circleRadius(imageFile, radius, rectTmp);
    if (!ret.success)
    {
        return ret;
    }
    m_radiusMap[fiducialTypeQ][fiducialNo] = radius;
    return Result();
}

cv::Mat FiducialRecognize::getImgDraw()
{
    return m_img_draw;
}

Result FiducialRecognize::circleDetect(cv::Mat src, cv::Point2f &center, FiducialInfo fiducialInfo)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    if (src.data == NULL)
    {
        return Result(false, "Image is empty.");
    }

    int multipleRect = 10;
    //if (!fiducialInfo.isCenter)
    //{
    //    fiducialInfo.isCenter = FiducialConfig::instance()->getFiducialCenter();
    //    multipleRect = 20;
    //}

    float radius;
    Result ret = getTmplRadius(fiducialInfo.fiducialType, fiducialInfo.fiducialNo, radius);
    if (!ret.success)
    {
        return ret;
    }        

    cv::Rect rect;
    //if (!fiducialInfo.isCenter)
    //{
    //    rect = cv::Rect(0, 0, src.cols, src.rows);
    //}
    //else
    {
        int width = radius * multipleRect;
        if (src.cols < width || src.rows < width)
        {
            rect = cv::Rect(0, 0, src.cols, src.rows);
        }
        else
        {
            rect = cv::Rect(src.cols / 2 - width / 2, src.rows / 2 - width / 2, width, width);
        }
    }

    ret = circleDetect(src, rect, radius, center);

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    // qWarning() << "---- takt time ---- " << takeTime << " ms...";
    return ret;
}

Result FiducialRecognize::circleDetectInputFiducial(cv::Mat src, cv::Point2f &center, std::string fiducialType)
{
    if (src.data == NULL)
    {
        return Result(false, "Image is empty.");
    }

    int fiducialNo = FiducialConfig::instance()->getInputFiducialNumber(fiducialType);
    float radius;
    Result ret = getTmplRadius(fiducialType, fiducialNo, radius);
    if (!ret.success)
    {
        return ret;
    }

    cv::Rect rect = cv::Rect(0, 0, src.cols, src.rows);
    ret = circleDetect(src, rect, radius, center);
    return ret;
}

Result FiducialRecognize::circleProjectorDetect(cv::Mat src, cv::Point2f &center)
{
    if (src.data == NULL)
    {
        return Result(false, "Image is empty.");
    }

    Result ret;
    if (m_radiusProjector == 0)
    {
        Rect rectTmp;
        ret = circleRadius("./config/templateImg/fiducial/projector_templete.tif", m_radiusProjector, rectTmp);
        if (!ret.success)
        {
            return ret;
        }
    }

    cv::Rect rect = cv::Rect(0, 0, src.cols, src.rows);
    ret = circleDetect(src, rect, m_radiusProjector, center, m_radiusProjector + 120);
    return ret;
}

Result FiducialRecognize::circleDetectTemp(cv::Mat src, cv::Point2f& center, FiducialInfo fiducialInfo)
{
    int startTime = QDateTime::currentMSecsSinceEpoch();

    if (src.data == NULL)
    {
        return Result(false, "Image is empty.");
    }

    cv::Mat temp;
    float radius;
    Result ret =  readTemp(radius, temp, fiducialInfo.fiducialType, fiducialInfo.fiducialNo);
    if (!ret.success)
    {
        return ret;
    }

    //Result ret = getTmplRadius(fiducialInfo.fiducialType, fiducialInfo.fiducialNo, radius);
    //if (!ret.success)
    //{
    //    return ret;
    //}

    int multipleRect = 10;
    cv::Rect rect;
    {
        int width = radius * multipleRect;
        if (src.cols < width || src.rows < width)
        {
            rect = cv::Rect(0, 0, src.cols, src.rows);
        }
        else
        {
            rect = cv::Rect(src.cols / 2 - width / 2, src.rows / 2 - width / 2, width, width);
        }
    }

    ret = circleDetectMatchTemp(src, temp, rect, radius, center);

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    // qWarning() << "---- takt time ---- " << takeTime << " ms...";
    return ret;
}

void FiducialRecognize::myfilter(Mat& image_input, Mat& image_output) //4
{
    image_output = image_input.clone();
    int la;
    for (int i = 1; i < (image_input.rows - 1); i++)
    {
        for (int j = 1; j < (image_input.cols - 1); j++)
        {
            la = 4 * image_input.at<uchar>(i, j) - image_input.at<uchar>(i + 1, j) - image_input.at<uchar>(i - 1, j) - image_input.at<uchar>(i, j + 1) - image_input.at<uchar>(i, j - 1);

            image_output.at<uchar>(i, j) = saturate_cast<uchar>(image_output.at<uchar>(i, j) + la);

        }
    }
}

void FiducialRecognize::myfilter2(Mat& image_input, Mat& image_output) //8
{
    image_output = image_input.clone();
    int la2;
    for (int i = 1; i < (image_input.rows - 1); i++)
    {
        for (int j = 1; j < (image_input.cols - 1); j++)
        {
            la2 = 8 * image_input.at<uchar>(i, j) - image_input.at<uchar>(i + 1, j) - image_input.at<uchar>(i - 1, j) - image_input.at<uchar>(i, j + 1) - image_input.at<uchar>(i, j - 1)
                - image_input.at<uchar>(i - 1, j - 1) - image_input.at<uchar>(i + 1, j + 1) - image_input.at<uchar>(i - 1, j + 1) - image_input.at<uchar>(i + 1, j - 1);

            image_output.at<uchar>(i, j) = saturate_cast<uchar>(image_output.at<uchar>(i, j) + la2);

        }
    }
    //cv::Laplacian();
}

void FiducialRecognize::test()
{
    QList<QString> list;
    list 
        << "1.tif"
         << "2.tif"
         << "3.tif"
         << "4.tif";
    for (QString file : list)
    {
        //qWarning() << "---- file name ---- " << file;

        Mat src = imread("D:\\MV\\ff\\" + file.toStdString(), IMREAD_UNCHANGED);
        if (src.empty())
        {
            return;
        }

        Rect rect = Rect(0, 0, src.cols, src.rows);
        int circleRadius = 58;
        cv::Point2f center;
        int circleRadiusMax = 68;
        FiducialInfo fiducialInfo;
        fiducialInfo.fiducialNo = 0;
        fiducialInfo.isCenter = false;
        circleDetect(src, center, fiducialInfo);
        //circleDetectTemp(src, center, fiducialInfo);

        //circleDetect(src, rect, circleRadius, center, circleRadiusMax);
        //circleDetectMatchTemp(src, rect, circleRadius, center, circleRadiusMax);

        //Mat src = imread("D:\\fiducial\\" + file.toStdString(), IMREAD_UNCHANGED);
        //if (src.empty())
        //{
        //    return;
        //}

        //cv::Point2f center;
        //Result ret = circleDetect(src, cv::Rect(0, 0, src.cols, src.rows), center);
        // if (!ret)
        //{
        //    return;
        //}
    }
}


// 判断闭合程度：轮廓是否普遍闭合（不闭合说明断边多，需要膨胀）
bool isEdgeMostlyOpen(const std::vector<std::vector<cv::Point>>& contours, int minPoints = 5) {
    int openContours = 0, total = 0;

    for (const auto& c : contours) {
        if (c.size() < minPoints) continue;
        double epsilon = 2.0;
        std::vector<cv::Point> approx;
        cv::approxPolyDP(c, approx, epsilon, true); // true = closed
        bool isClosed = cv::isContourConvex(approx);
        if (!isClosed) openContours++;
        total++;
    }

    if (total == 0) return true; // 全部断裂
    float openRatio = openContours * 1.0f / total;
    return openRatio > 0.3f; // 超过30%的轮廓不闭合，认为“整体断裂严重”
}

// 自动膨胀策略：根据断边情况决定是否 dilate
cv::Mat autoDilateEdges(const cv::Mat& gray, bool visualize) {
    cv::Mat blurred, sharp, canny;

    // 增强对比度 + 锐化
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 1.5);
    cv::addWeighted(gray, 1.5, blurred, -0.5, 0, sharp);

    // Canny 边缘
    cv::Canny(sharp, canny, 50, 150);

    // 判断是否断边严重
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(canny.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    bool shouldDilate = isEdgeMostlyOpen(contours);

    if (shouldDilate) {
        cv::dilate(canny, canny, cv::getStructuringElement(cv::MORPH_ELLIPSE, { 3,3 }));
        if (visualize) std::cout << "[INFO] 自动检测到边缘破损严重，已应用膨胀处理。" << std::endl;
    }
    else {
        if (visualize) std::cout << "[INFO] 边缘结构良好，跳过膨胀。" << std::endl;
    }

    if (visualize) {
        cv::imshow("Canny Edge (Auto Dilated)", canny);
        cv::waitKey(0);
    }

    return canny;
}


Result FiducialRecognize::circleDetectMatchTemp(cv::Mat src, cv::Mat temp, Rect rect, int circleRadius, cv::Point2f& center, int circleRadiusMax)
{
    Mat grayImage;
    if (src.channels() == 4)
    {
        cvtColor(src, grayImage, COLOR_BGRA2GRAY);
    }
    else if (src.channels() == 3)
        cvtColor(src, grayImage, COLOR_BGR2GRAY);
    else
        grayImage = src.clone();
    grayImage = grayImage(rect);

    cv::Mat color;
    cv::cvtColor(grayImage, color, cv::COLOR_GRAY2RGB);

    int startTime = QDateTime::currentMSecsSinceEpoch();
    cv::Mat filterImg;
    //myfilter2(grayImage, filterImg);
    {
        // 4. 锐化（增强边缘）
        Mat sharp;
        Mat kernel = (Mat_<float>(3, 3) <<
            0, -1, 0,
            -1, 5, -1,
            0, -1, 0);
        filter2D(grayImage, sharp, grayImage.depth(), kernel);

        Mat enhanced;
        GaussianBlur(grayImage, enhanced, Size(3, 3), 0);
        Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
        clahe->apply(enhanced, enhanced);
        Mat sharpened;
        Mat sharpKernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
        filter2D(enhanced, sharpened, enhanced.depth(), sharpKernel);

        filterImg = sharp;
    }

    Mat blurImage;
    GaussianBlur(filterImg, blurImage, Size(5, 5), 1.5, 1.5);  // σ由ksize自动计算

    Mat edgesImage;
    Canny(blurImage, edgesImage, 30, 90);
    cv::dilate(edgesImage, edgesImage, cv::Mat(), Point(-1, -1), 1);

    if(0){
        Mat blurImage;
        GaussianBlur(filterImg, blurImage, Size(3, 3), 0, 0);  // σ由ksize自动计算

        Mat edgesImage;
        Canny(blurImage, edgesImage, 30, 90);
        //cv::Mat DeImage = autoDilateEdges(edgesImage, true);
        //cv::dilate(edgesImage, edgesImage, cv::Mat(), Point(-1, -1), 1);

        std::vector<std::vector<cv::Point>> contours;
        findContours(edgesImage, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            //if (contour.size() < 5) continue;  // fitEllipse要求至少5个点

            // 拟合椭圆
            cv::RotatedRect ellipse = cv::fitEllipse(contour);

            // 获取圆心、半径信息
            cv::Point2f centerTmp = ellipse.center;
            cv::Size2f axes = ellipse.size;   // axes.width: 长轴，axes.height: 短轴
            float angle = ellipse.angle;      // 椭圆旋转角度

            if(axes.width / 2 < circleRadius - 10 || axes.width / 2 > circleRadius + 10){
                continue;
            }

            // 圆度评估（判断是否接近圆）
            float ratio = std::min(axes.width, axes.height) / std::max(axes.width, axes.height);
            if (ratio > 0.90) {  // 判断是否“近似圆”
                // 可视化：画椭圆和圆心
                cv::ellipse(color, ellipse, cv::Scalar(0, 255, 0), 1);
                cv::circle(color, centerTmp, 2, cv::Scalar(0, 0, 255), -1);

                center = centerTmp + cv::Point2f(rect.x, rect.y);

                std::cout << "圆心: (" << center.x << ", " << center.y << ") 半径: "
                    << (axes.width + axes.height) / 4 << std::endl;
            }
        }

        return Result();
    }

    Mat tpl = createHollowCircleTemplate(circleRadius * 2, 9);
    //Mat tpl = temp;

    Mat result;
    matchTemplate(edgesImage, tpl, result, TM_CCOEFF_NORMED);

    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    if (maxVal < 0.5) 
    {
        rectangle(color, Rect(maxLoc.x, maxLoc.y, tpl.cols, tpl.rows), Scalar(255, 0, 0), 1);
        qWarning() << "Match found with confidence: " << maxVal;
        return Result(false, "Fiducial auto detect error.");
    }

    // 计算匹配中心 = 左上角 + 模板中心偏移
    cv::Point2f centerTmp = cv::Point2f(maxLoc.x + tpl.cols / 2.0f, maxLoc.y + tpl.rows / 2.0f);
    circle(color, center, 2, Scalar(0, 0, 255), -1); // 标出圆心

    center = centerTmp + cv::Point2f(rect.x, rect.y);
    return Result();
}

// 创建空心圆模板
Mat FiducialRecognize::createHollowCircleTemplate(int size, int thickness)
{
    Mat tpl = Mat::zeros(size, size, CV_8UC1);
    Point center(size / 2, size / 2);
    int radius = size / 2 ;

    circle(tpl, center, radius, Scalar(255), thickness);
    return tpl;
}

Result FiducialRecognize::readTemp(float& radius, cv::Mat& temp, string fiducialType, int fiducialNo)
{
    QString fiducialTypeQ = QString::fromStdString(fiducialType);
    if (m_tempMap.contains(fiducialTypeQ) && m_tempMap[fiducialTypeQ].contains(fiducialNo)) {
        temp = m_tempMap[fiducialTypeQ][fiducialNo];
        return Result();
    }

    std::string imageFile = FiducialConfig::instance()->getTempleteFilePath(fiducialType, fiducialNo);
    Mat src;
    src = imread(imageFile, IMREAD_UNCHANGED);
    if (src.empty())
    {
        return Result(false, "Detect fiducial circle templete error, image is empty.");
    }

    Mat grayImage;
    if (src.channels() == 4)
    {
        cvtColor(src, grayImage, COLOR_BGRA2GRAY);
    }
    else if (src.channels() == 3)
        cvtColor(src, grayImage, COLOR_BGR2GRAY);
    else
        grayImage = src.clone();

    int startTime = QDateTime::currentMSecsSinceEpoch();
    Mat filterImg;
    myfilter2(grayImage, filterImg);

    int takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    //qWarning() << "---- myfilter2 ====== " << takeTime << " ms...";

    Mat blurImage;
    GaussianBlur(filterImg, blurImage, Size(9, 9), 2, 2);
    Mat edgesImage;
    //Canny(blurImage, edgesImage, 1, 10);

    Mat blurImageT;
    cv::GaussianBlur(filterImg, blurImageT, cv::Size(3, 3), 0, 0);
    cv::threshold(blurImageT, edgesImage, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

    vector<Vec3f> circles;
    int minDist = 30;
    int param1 = 10;  // 10
    int param2 = 35;  // 35
    int minRadius = grayImage.cols / 4;
    int maxRadius = grayImage.cols / 2;
    HoughCircles(blurImage, circles, HOUGH_GRADIENT, 1, minDist, param1, param2, minRadius, maxRadius);

    takeTime = QDateTime::currentMSecsSinceEpoch() - startTime;
    //qWarning() << "---- HoughCircles ====== " << takeTime << " ms...";

    cv::Point2f center;
    cv::Rect rect = Rect(0,0, grayImage.cols, grayImage.rows);
    cv::cvtColor(src, m_img_draw, cv::COLOR_GRAY2RGB);
    for (size_t i = 0; i < circles.size(); i++)
    {
        Point centerTmp(cvRound(circles[i][0]), cvRound(circles[i][1]));
        center = centerTmp + Point(rect.x, rect.y);
        circle(m_img_draw, center, 2, Scalar(0, 0, 255), -1, 8, 0);

        radius = cvRound(circles[i][2]);
        circle(m_img_draw, center, radius, Scalar(0, 0, 255), 1, 8, 0);
    }

    if (circles.size() != 1)
    {
        return Result(false,
            QString("Detect circle error, circle size is %1.").arg(circles.size()).toStdString(),
            circles.size() && param2 > 35);
    }

    // 决定裁剪大小（建议比圆稍大一点）
    int size = radius * 2 + 10;
    Rect roi(center.x - size / 2, center.y - size / 2, size, size);
    roi &= Rect(0, 0, src.cols, src.rows);
    temp = edgesImage(roi).clone();
    return Result();
}
