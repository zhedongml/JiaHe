#include "ml_crosshairpara.h"
#include "ml_rectangleDetection.h"
#include <fstream>
#include <iostream>
#include"ml_multiCrossHairDetection.h"
using namespace cv;
// using namespace algorithm;
algorithm::MLCrosshairPara::MLCrosshairPara()
{
}

algorithm::MLCrosshairPara::~MLCrosshairPara()
{
}

void algorithm::MLCrosshairPara::SetMultCrosshairImage(Mat img_multcross)
{
    m_imgCrosshair = img_multcross;
    hydrusConfig.GetNewPara(img_multcross);
    m_para = hydrusConfig.m_para;
}

FovOffestRe algorithm::MLCrosshairPara::GetBoresight()
{
    FovOffestRe result;
    //result = Boresight5(m_imgCrosshair);
    result = BoresightNoBorder(m_imgCrosshair);
    return result;
}

FovAngleRe algorithm::MLCrosshairPara::GetFOVAngle()
{
    FovAngleRe result;
   // result = FOVAngle(m_imgCrosshair);
    result = FOVAngleSolid(m_imgCrosshair);
    return result;
}

double algorithm::MLCrosshairPara::GetcrossRotation()
{
    double result = 0;
    //result = crossRotation(m_imgCrosshair);
    result = crossRotationNoBorder(m_imgCrosshair);
    return result;
}

cv::Mat algorithm::MLCrosshairPara::getImgDraw()
{
    return m_img_draw;
}

float algorithm::MLCrosshairPara::getCrosshairRotationAngle()
{
    return m_rotationAngle;
}

vector<cv::Point2f> algorithm::MLCrosshairPara::getNineCrosshairMTFROICenter(cv::Mat img)
{
    vector<cv::Point2f> re;
    if (img.data != NULL)
    {
        // celeConfig.GetNewPara(img);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        cv::Rect rec, rect;
        cv::Mat img8 = convertToUint8(img);
        rec = findborder(img8);
        if (rec.x != 0 || rec.y != 0)
            rect = rec;
        else
            rect = m_para.rect;
        int m_roiStart = HydrusParameters::roiStart;
        cv::Point2f m_opticalcenter = HydrusParameters::W_ND0_opticalcenter;
        double m_pixel_size = m_para.pixel_size;
        double m_FocalLength = HydrusParameters::FocalLength;
        cv::Mat img_draw;
        img_draw = convertToUint8(img);
        cv::Mat roi = GetROIMat(img8, rect);
        vector<cv::Rect> rectVec;
        vector<cv::Mat> splitImg = ImageSegmentation(roi, rectVec);
        if (splitImg.size() == 9)
        {
            int offset = HydrusParameters::roioffset;
            CrossCenter aa(splitImg[4], offset);
            // cv::Rect rectMat = rectVec[4];
            int row = roi.rows;
            int col = roi.cols;
            int flag = HydrusParameters::crosshairAlgortihm;
            cv::Point2f center = aa.get_crossCenter(1);
            cv::Point2f realcenter;
            if (center.x != 0 && center.y != 0)
            {
                vector<double> Vline = aa.GetVerticalLine();
                vector<double> Hline = aa.GetHorizontalLine();
                int len = roi.rows;
                int width = roi.cols;

                int binNum = sqrt(7920 * 6004 / img.total());
                double dis = m_dis / binNum;
                double kh = -Hline[0] / Hline[1];
                double kv = -Vline[0] / Vline[1];
                double thetaH = atan(kh);
                double thetaV = atan(kv);
                cv::Point2f p1 = center + cv::Point2f(rectVec[4].x, rectVec[4].y) + cv::Point2f(rect.x, rect.y);
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
            }
        }
    }
    return re;
}

void algorithm::MLCrosshairPara::BoresightBatchTest()
{
    string dir = "E:\\project\\hydrus2Imgs\\20240111_CRARepeat3_MetricsTest\\1\\";
    std::string IMG_PATH = dir + "*.tif";
    //"E:\\LiNing\\十字线检测\\处理异常图\\*.tiff";//遍历文件夹下的所有.jpg文件
    // write file
    ofstream ofs;
    string txtdir = dir + "boresight.txt";
    ofs.open(txtdir, ios::out); // change name
    vector<cv::String> filenames;
    cv::glob(IMG_PATH, filenames);
    for (int num = 0; num < filenames.size(); num++)
    {
        //获取路径下的文件名
        const size_t last_idx = filenames[num].rfind('\\');
        string basename = filenames[num].substr(last_idx + 1);
        cout << num + 1 << "=" << basename << endl;
        string dir1 = dir + basename;
        cv::Mat img = cv::imread(dir1, -1);
        cv::Mat roi = img(cv::Rect(880, 660, 200, 200));
        CrossCenter ch;
        cv::Point2f c = ch.find_centerGaussian(roi);
        // SetMultCrosshairImage(img);
        // FovOffestRe re=GetBoresight();
        cv::Mat img_draw = convertToUint8(roi);
        circle(img_draw, c, 2, Scalar(255, 0, 255), -1);
        // getImgDraw();
        string writename = dir1 + "boresight.tiff"; // change name
        cv::imwrite(writename, img_draw);
        ofs << basename << "," << c << endl;
        // ofs << basename << ','<< re.p1 << "," << re.p2 << endl;
    }
}

void algorithm::MLCrosshairPara::SetRGBImg(cv::Mat imgR, cv::Mat imgG, cv::Mat imgB)
{
    m_imgRGB.imgR = imgR;
    m_imgRGB.imgG = imgG;
    m_imgRGB.imgB = imgB;
    hydrusConfig.GetNewPara(imgR);
    m_para = hydrusConfig.m_para;
}

LaterColorRe algorithm::MLCrosshairPara::GetLateralColorAvg()
{
    LaterColorRe result;
    result = LateralColor(m_imgRGB);
    return result;
}
std::vector<cv::Mat> algorithm::MLCrosshairPara::GetLateralColorMat()
{
    std::vector<cv::Mat> re;
    if (m_laterColarRe.RGmat.data != NULL && m_laterColarRe.RBmat.data != NULL && m_laterColarRe.BGmat.data != NULL)
    {
        re.push_back(m_laterColarRe.RGmat);
        re.push_back(m_laterColarRe.RBmat);
        re.push_back(m_laterColarRe.BGmat);
    }
    else
    {
        LateralColor(m_imgRGB);
        re.push_back(m_laterColarRe.RGmat);
        re.push_back(m_laterColarRe.RBmat);
        re.push_back(m_laterColarRe.BGmat);
    }
    return re;
}
cv::Mat algorithm::MLCrosshairPara::GetLateralColorMax()
{
    cv::Mat max, src;
    if (m_laterColarRe.maxBG != -1 && m_laterColarRe.maxRB != -1 && m_laterColarRe.maxRG != -1)
    {
        src = (Mat_<double>(3, 1) << m_laterColarRe.maxRG, m_laterColarRe.maxRB, m_laterColarRe.maxBG);
    }
    else
    {
        LateralColor(m_imgRGB);
        src = (Mat_<double>(3, 1) << m_laterColarRe.maxRG, m_laterColarRe.maxRB, m_laterColarRe.maxBG);
    }
    src.convertTo(max, CV_32FC1);
    return max;
}
cv::Mat algorithm::MLCrosshairPara::getImgRDraw()
{
    return m_imgR_draw;
}

cv::Mat algorithm::MLCrosshairPara::getImgGDraw()
{
    return m_imgG_draw;
}

cv::Mat algorithm::MLCrosshairPara::getImgBDraw()
{
    return m_imgB_draw;
}

vector<cv::Point2f> algorithm::MLCrosshairPara::getRotationPoints()
{
    return m_rotaionPts;
}

void algorithm::MLCrosshairPara::SetFovInform(float centerX, float centerY, std::vector<double> rotateH,
                                              std::vector<double> rotateV)
{
    m_center.x = centerX;
    m_center.y = centerY;
    m_Hline = rotateH;
    m_Vline = rotateV;
}
FOVInf algorithm::MLCrosshairPara::GetFovInform()
{
    FOVInf fovinf;
    fovinf.centerX = m_center.x;
    fovinf.centerY = m_center.y;
    fovinf.Hline = m_Hline;
    fovinf.Vline = m_Vline;
    return fovinf;
}

vector<cv::Point2f> algorithm::MLCrosshairPara::GetFovEdgePoints()
{
    if (m_fov.EdgePoint.size() > 0)
    {
        return m_fov.EdgePoint;
    }
    else
    {
        FOVAngle(m_imgCrosshair);
        return m_fov.EdgePoint;
    }
}

FovOffestRe algorithm::MLCrosshairPara::Boresight(cv::Mat img)
{
    if (img.data != NULL)
    {
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);

        // celeConfig.GetNewPara(img);
        cv::Rect rec, rect;
        cv::Mat img8 = convertToUint8(img);
        rec = findborder(img8);
        if (rec.x != 0 || rec.y != 0)
            rect = rec;
        else
            rect = m_para.rect;
        int m_roiStart = HydrusParameters::roiStart;
        cv::Point2f m_opticalcenter = HydrusParameters::W_ND0_opticalcenter;
        double m_pixel_size = m_para.pixel_size;
        double m_FocalLength = HydrusParameters::FocalLength;
        cv::Mat img_draw;
        img_draw = convertToUint8(img);
        cv::Mat roi = GetROIMat(img8, rect);
        vector<cv::Rect> rectVec;
        vector<cv::Mat> splitImg = ImageSegmentationTemplate1(roi, rectVec);
        // vector<cv::Mat> splitImg = ImageSegmentation(roi);
        // for (int i=1;i<9;i++)
        // rectVec.push_back(cv::Rect(0, 0, 0, 0));

        if (splitImg.size() == 9)
        {
            int offset = HydrusParameters::roioffset;
            CrossCenter aa(splitImg[4], offset);
            cv::Rect rectMat = rectVec[4];
            int row = roi.rows;
            int col = roi.cols;
            int flag = HydrusParameters::crosshairAlgortihm;
            cv::Point2f center = aa.get_crossCenter(2);
            cv::Point2f realcenter;
            if (center.x != 0 && center.y != 0)
            {
                realcenter.x = center.x + rectMat.x + m_roiStart + rect.x;
                realcenter.y = center.y + rectMat.y + m_roiStart + rect.y;
                // draw
                m_Vline = aa.GetVerticalLine();
                m_Hline = aa.GetHorizontalLine();
                m_roi_center = aa.get_roi_center();
                int deltaPx = realcenter.x - m_opticalcenter.x / hydrusConfig.m_para.binNum;
                int deltaPy = realcenter.y - m_opticalcenter.y / hydrusConfig.m_para.binNum;
                m_fovoff.V = atan((m_pixel_size * deltaPy) / m_FocalLength) * 180.0 / CV_PI; // degree
                m_fovoff.H = atan((m_pixel_size * deltaPx) / m_FocalLength) * 180.0 / CV_PI; // degree
                double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
                m_fovoff.D = atan((m_pixel_size * deltaP) / m_FocalLength) * 180.0 / CV_PI; // degree
                // TODO: why *binNum
                m_center = realcenter * m_para.binNum;
                // draw, can delete
                // cout << realcenter << endl;
                circle(img_draw, realcenter, 5, cv::Scalar(255, 0, 0), -1);
                circle(img_draw, m_opticalcenter / m_para.binNum, 5, cv::Scalar(255, 0, 255), -1);
                m_img_draw = img_draw.clone();
            }
        }
    }
    return m_fovoff;
}

FovOffestRe algorithm::MLCrosshairPara::Boresight5(cv::Mat img)
{
    if (img.data != NULL)
    {
        // celeConfig.GetNewPara(img);
        cv::Mat img_draw;
        img_draw = convertToUint8(img);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        cv::Rect rec, rect;
        cv::Mat img8 = convertToUint8(img);
        cv::Mat roi1 = getRectROIImg(img, m_para.rect);
        rec = findborder(roi1);
        if (rec.x != 0 || rec.y != 0)
        {
            rect.x = rec.x + m_para.rect.x;
            rect.y = rec.y + m_para.rect.y;
            rect.width = rec.width;
            rect.height = rec.height;
        }
        else
            rect = m_para.rect;

        int m_roiStart = HydrusParameters::roiStart;
        cv::Point2f m_opticalcenter = HydrusParameters::W_ND0_opticalcenter;
        double m_pixel_size = m_para.pixel_size;
        double m_FocalLength = HydrusParameters::FocalLength;

        cv::Mat roi = GetROIMat(img8, rect);
        vector<cv::Rect> rectVec;
        vector<cv::Mat> splitImg = ImageSegmentation(roi, rectVec);
        if (splitImg.size() == 9)
        {
            int offset = HydrusParameters::roioffset;
            CrossCenter aa(splitImg[4], offset);
            // cv::Rect rectMat = rectVec[4];
            int row = roi.rows;
            int col = roi.cols;
            int flag = HydrusParameters::crosshairAlgortihm;
            cv::Point2f center = aa.get_crossCenter(2);
            cv::Point2f realcenter;
            if (center.x != 0 && center.y != 0)
            {
                realcenter.x = center.x + col / 3.0 + m_roiStart + rect.x;
                realcenter.y = center.y + row / 3.0 + m_roiStart + rect.y;

                // add roataion correction
                double theta = rotationAngle * CV_PI / 180;
                double x2 = img.cols / 2;
                double y2 = img.rows / 2;
                double x1 = realcenter.x;
                double y1 = realcenter.y;
                double x = (x1 - x2) * cos(theta) - (y1 - y2) * sin(theta) + x2;
                double y = (x1 - x2) * sin(theta) + (y1 - y2) * cos(theta) + y2;
                realcenter = cv::Point2f(x, y);
                // draw
                m_Vline = aa.GetVerticalLine();
                m_Hline = aa.GetHorizontalLine();
                m_rotationAngle = atan(-m_Hline[0] / m_Hline[1]) * 180 / CV_PI;
                m_roi_center = aa.get_roi_center();
                int deltaPx = realcenter.x - m_opticalcenter.x / hydrusConfig.m_para.binNum;
                int deltaPy = realcenter.y - m_opticalcenter.y / hydrusConfig.m_para.binNum;
                m_fovoff.V = atan((m_pixel_size * deltaPy) / m_FocalLength) * 180.0 / CV_PI; // degree
                m_fovoff.H = atan((m_pixel_size * deltaPx) / m_FocalLength) * 180.0 / CV_PI; // degree
                double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
                m_fovoff.D = atan((m_pixel_size * deltaP) / m_FocalLength) * 180.0 / CV_PI; // degree
                // TODO: why *binNum
                m_center = realcenter * m_para.binNum;
                // draw, can delete
                // cout << realcenter << endl;

                circle(img_draw, realcenter, 5, cv::Scalar(255, 0, 0), -1);
                circle(img_draw, m_opticalcenter / m_para.binNum, 5, cv::Scalar(255, 0, 255), -1);
                m_img_draw = img_draw.clone();
                // m_fovoff.p1 = realcenter;
                // m_fovoff.p2 = m_opticalcenter / m_para.binNum;
            }
        }
    }
    return m_fovoff;
}
FovOffestRe algorithm::MLCrosshairPara::BoresightNoBorder(cv::Mat img)
{
    FovOffestRe re;
    if (img.data != NULL)
    {
        m_img_draw.release();
        // celeConfig.GetNewPara(img);
        cv::Mat img_draw;
        img_draw = convertToUint8(img);
        img_draw = convertTo3Channels(img_draw);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        cv::Mat img8 = convertToUint8(img);
        cv::Mat roi1 = getRectROIImg(img, m_para.rect);
        cv::Point2f m_opticalcenter = HydrusParameters::W_ND0_opticalcenter;
        double m_pixel_size = m_para.pixel_size;
        double m_FocalLength = HydrusParameters::FocalLength;
        MultiCrossHairDetection md;
        CenterCrossHairRe centerRe=md.getCenterCrossHairCenter(roi1);
        if (centerRe.flag.success)
        {
            cv::Mat cenMat = roi1(centerRe.rectCenter).clone();
            CrossCenter cc;
            
            cv::Point2f center;
            if (isSLB)
                center = cc.find_centerLINES(cenMat);
            else
                center = cc.find_centerGaussian(cenMat);
            //cv::Point2f center=cc.find_centerLINES(cenMat);
            cv::Point2f realcenter;
            int m_roiStart = HydrusParameters::roiStart;
            if (center.x>1e-6&&center.y>1e-6)
            {
                realcenter.x = center.x + m_roiStart + m_para.rect.x + centerRe.rectCenter.x;
                realcenter.y = center.y + m_roiStart + m_para.rect.y + centerRe.rectCenter.y;
                // add roataion correction
                double theta = rotationAngle * CV_PI / 180;
                double x2 = img.cols / 2;
                double y2 = img.rows / 2;
                double x1 = realcenter.x;
                double y1 = realcenter.y;
                double x = (x1 - x2) * cos(theta) - (y1 - y2) * sin(theta) + x2;
                double y = (x1 - x2) * sin(theta) + (y1 - y2) * cos(theta) + y2;
                realcenter = cv::Point2f(x, y);
                m_Vline = cc.GetVerticalLine();
                m_Hline = cc.GetHorizontalLine();
                //m_rotationAngle = atan(-m_Hline[0] / m_Hline[1]) * 180 / CV_PI;
                m_roi_center = cc.get_roi_center();
                int deltaPx = realcenter.x - m_opticalcenter.x / hydrusConfig.m_para.binNum;
                int deltaPy = realcenter.y - m_opticalcenter.y / hydrusConfig.m_para.binNum;
                re.V = atan((m_pixel_size * deltaPy) / m_FocalLength) * 180.0 / CV_PI; // degree
                re.H = atan((m_pixel_size * deltaPx) / m_FocalLength) * 180.0 / CV_PI; // degree
                double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
                re.D = atan((m_pixel_size * deltaP) / m_FocalLength) * 180.0 / CV_PI; // degree
                m_center = realcenter * m_para.binNum;
                re.pixelx = realcenter.x;
                re.pixely = realcenter.y;

                cv::circle(img_draw, realcenter, 16 / m_para.binNum, cv::Scalar(255, 0, 0), -1);
                cv::circle(img_draw, m_opticalcenter / m_para.binNum, 16 / m_para.binNum, cv::Scalar(255, 0, 255), -1);
              
                string xstr = to_string(realcenter.x);
                string ystr = to_string(realcenter.y);
                string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);

                cv::putText(img_draw, text, realcenter, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 2);

                
                
                m_img_draw = img_draw.clone();
                m_fovoff = re;
                re.crossCenter.push_back(realcenter);
                re.crossCenter.push_back(m_opticalcenter);
            }
        }
        }    
    return re;
}
FovOffestRe algorithm::MLCrosshairPara::BoresightNoBorder(cv::Mat img, int roationAngle, MirrorALG mirror)
{
    FovOffestRe re;
    if (img.data != NULL)
    {
        m_img_draw.release();
        // celeConfig.GetNewPara(img);
        cv::Mat img_draw;
        img_draw = convertToUint8(img);
        img_draw = convertTo3Channels(img_draw);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        cv::Mat img8 = convertToUint8(img);
        cv::Mat roi1 = getRectROIImg(img, m_para.rect);
        cv::Point2f m_opticalcenter = HydrusParameters::W_ND0_opticalcenter;
        double m_pixel_size = m_para.pixel_size;
        double m_FocalLength = HydrusParameters::FocalLength;
        MultiCrossHairDetection md;
        CenterCrossHairRe centerRe = md.getCenterCrossHairCenter(roi1);
        if (centerRe.flag.success)
        {
            cv::Mat cenMat = roi1(centerRe.rectCenter).clone();
            CrossCenter cc;
           // cv::Point2f center = cc.find_centerLINES(cenMat);
            cv::Point2f center;
            if (isSLB)
                center = cc.find_centerLINES(cenMat);
            else
                center = cc.find_centerGaussian(cenMat);

            cv::Point2f realcenter;
            int m_roiStart = HydrusParameters::roiStart;
            if (center.x > 1e-6 && center.y > 1e-6)
            {
                realcenter.x = center.x + m_roiStart + m_para.rect.x + centerRe.rectCenter.x;
                realcenter.y = center.y + m_roiStart + m_para.rect.y + centerRe.rectCenter.y;
                // add roataion correction
                double theta = rotationAngle * CV_PI / 180;
                double x2 = img.cols / 2;
                double y2 = img.rows / 2;
                double x1 = realcenter.x;
                double y1 = realcenter.y;
                double x = (x1 - x2) * cos(theta) - (y1 - y2) * sin(theta) + x2;
                double y = (x1 - x2) * sin(theta) + (y1 - y2) * cos(theta) + y2;
                realcenter = cv::Point2f(x, y);
                m_Vline = cc.GetVerticalLine();
                m_Hline = cc.GetHorizontalLine();
                // m_rotationAngle = atan(-m_Hline[0] / m_Hline[1]) * 180 / CV_PI;
                m_roi_center = cc.get_roi_center();
                MLimagePublic pl;
                cv::Point2f basePoint = pl.updateOpticalCenter(m_opticalcenter / hydrusConfig.m_para.binNum, img.size(),
                                                               roationAngle, mirror);
                int deltaPx = realcenter.x - basePoint.x;
                int deltaPy = realcenter.y - basePoint.y;
                re.V = atan((m_pixel_size * deltaPy) / m_FocalLength) * 180.0 / CV_PI; // degree
                re.H = atan((m_pixel_size * deltaPx) / m_FocalLength) * 180.0 / CV_PI; // degree
                double deltaP = sqrt(pow(deltaPx, 2) + pow(deltaPy, 2));
                re.D = atan((m_pixel_size * deltaP) / m_FocalLength) * 180.0 / CV_PI; // degree
                m_center = realcenter * m_para.binNum;
                re.pixelx = realcenter.x;
                re.pixely = realcenter.y;

                cv::circle(img_draw, realcenter, 4 / m_para.binNum, cv::Scalar(255, 0, 0), -1);
                cv::circle(img_draw, basePoint, 20 / m_para.binNum, cv::Scalar(255, 0, 255), -1);

               // cv::circle(img_draw, m_opticalcenter / m_para.binNum, 20 / m_para.binNum, cv::Scalar(255, 0, 255), -1);
                m_img_draw = img_draw.clone();
                m_fovoff = re;
            }
        }
    }
    return re;
}
void algorithm::MLCrosshairPara::setIsSLB(bool flag)
{
    isSLB = flag;
}
FovAngleRe algorithm::MLCrosshairPara::FOVAngle(cv::Mat img)
{
    if (img.data != NULL)
    {
        // GetNewPara(img);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        img = getRotationImg(img, m_rotationAngle);
        int bin = m_para.binNum;
        cv::Rect rec, rect;
        cv::Mat img8 = convertToUint8(img);
        cv::Mat roi1 = getRectROIImg(img, m_para.rect);
        rec = findborder(roi1);
        if (rec.x != 0 || rec.y != 0)
        {
            rect.x = rec.x + m_para.rect.x;
            rect.y = rec.y + m_para.rect.y;
            rect.width = rec.width;
            rect.height = rec.height;
        }
        else
            rect = m_para.rect;
        m_fov.roiRect = rect;
        int m_roiStart = HydrusParameters::roiStart;
        cv::Mat img_draw = img8.clone();
        cv::Mat roi = GetROIMat(img8, rect);
        int row = roi.rows;
        int col = roi.cols;
        vector<cv::Rect> rectVec;
        // vector<cv::Mat> splitImg = ImageSegmentationTemplate1(roi, rectVec); // ImageSegmentation(roi);
        vector<cv::Mat> splitImg = ImageSegmentation(roi, rectVec);

        cv::Point2f center, realcenter;
        vector<double> Vline, Hline;
        Point end;
        vector<Point2f> imgend;
        if (splitImg.size() == 9)
        {
            for (int i = 1; i < splitImg.size(); i = i + 2)
            {
                cv::Mat tmp = splitImg[i];
                cv::Rect rectMat = rectVec[i];
                //  rectMat.x = rectMat.x - 60;
                //  rectMat.y = rectMat.y - 60;
                //  rectMat.width = rectMat.width + 120;
                //  rectMat.height = rectMat.height + 120;
                //  cv::Rect roiRect = cv::Rect(0, 0, roi.cols, roi.rows);
                //  cv::Rect andRect = rectMat & roiRect;
                //  // add
                //  //TODO:
                ////  tmp = roi(andRect);
                CrossCenter cc(tmp, 10);
                int flag = HydrusParameters::crosshairAlgortihm;
                //  center = getendpointHoughLine(tmp,i); // getendpointTemplate(tmp, i); // cc.get_crossCenter(flag);
                center = getendpointTest(tmp);
                if (center.x >= 0 && center.y >= 0)
                {
                    realcenter.x = center.x + rectMat.x + m_roiStart + rect.x;
                    realcenter.y = center.y + rectMat.y + m_roiStart + rect.y;
                    circle(img_draw, realcenter, 2, Scalar(255, 0, 255), -1);
                    imgend.push_back(realcenter);
                }
            }
            cv::Point2f cen;
            // update,need to calculate in real time
            // GetBoresight();
            CrossCenter cc1(splitImg[4], 10);
            cen = cc1.find_centerGaussian(splitImg[4]);
            cen.x = cen.x + rectVec[4].x + m_roiStart + rect.x;
            cen.y = cen.y + rectVec[4].y + m_roiStart + rect.y;

            // if (m_center.x != 0 && m_center.y != 0)
            //{
            //    cen.x = m_center.x / bin;
            //    cen.y = m_center.y / bin;
            //}
            // else {
            //    GetBoresight();
            //    cen.x = m_center.x / bin;
            //    cen.y = m_center.y / bin;
            //}

            if (imgend.size() == 4 && cen.x > 0 && cen.y > 0)
            {
                for (int i = 0; i < imgend.size(); i++)
                {
                    circle(img_draw, imgend[i], 2, Scalar(255, 0, 255), -1);
                }
                double disH = Getdistance(imgend[0], cen);
                double disH1 = Getdistance(imgend[3], cen);
                double disV = Getdistance(imgend[1], cen);
                double disV1 = Getdistance(imgend[2], cen);
                circle(img_draw, cen, 2, Scalar(255, 0, 255), -1);
                double m_FocalLength = HydrusParameters::FocalLength;
                double m_pixel_size = m_para.pixel_size;
                double FovH = atan(m_pixel_size * disH / m_FocalLength) * 180.0 / CV_PI;
                double FovH1 = atan(m_pixel_size * disH1 / m_FocalLength) * 180.0 / CV_PI;
                double FovV = atan(m_pixel_size * disV / m_FocalLength) * 180.0 / CV_PI;
                double FovV1 = atan(m_pixel_size * disV1 / m_FocalLength) * 180.0 / CV_PI;
                m_fov.FovH = FovH + FovH1;
                m_fov.FovV = FovV + FovV1;
                m_fov.EdgePoint = imgend;
                m_img_draw = img_draw.clone();
            }
        }
    }
    return m_fov;
}
FovAngleRe algorithm::MLCrosshairPara::FOVAngleSolid(cv::Mat img)
{
    FovAngleRe re;
    if (img.data != NULL)
    {
        // GetNewPara(img);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        img = getRotationImg(img, m_rotationAngle);
        int bin = m_para.binNum;
        cv::Rect rec, rect;
        cv::Mat img8 = convertToUint8(img);
        cv::Mat roi1 = getRectROIImg(img8, m_para.rect);
        //img8(m_para.rect).clone();
        RectangleDetection rd;
        cv::RotatedRect rectR = rd.getRectangleBorder(roi1);
        //roi1 = getRotationImg(roi1, rectR.angle);
        cv::Mat img_draw = convertToUint8(roi1);
        img_draw = convertTo3Channels(img_draw);
        cv::Point2f P[4];
        rectR.points(P);
        cv::Point2f cen = rectR.center;
        cv::Point2f h1 = (P[0] + P[1]) / 2.0;
        cv::Point2f h2 = (P[2] + P[3]) / 2.0;
        cv::Point2f v1 = (P[0] + P[3]) / 2.0;
        cv::Point2f v2 = (P[2] + P[1]) / 2.0;

        cv::Mat rowMat = roi1(cv::Range(cen.y, cen.y + 1),cv::Range(0, roi1.cols));
        cv::Mat colMat = roi1(cv::Range(0, roi1.rows), cv::Range(cen.x,cen.x+1));
        if(findEdgePts(rowMat, 0, h1, h2))
        {
            h1.y = cen.y;
            h2.y = cen.y;
        }
 
        if(findEdgePts(colMat, 1, v1, v2))
        {
            v1.x = cen.x;
            v2.x = cen.x;  
        }
        re.EdgePoint.push_back(h1);
        re.EdgePoint.push_back(h2);
        re.EdgePoint.push_back(v1);
        re.EdgePoint.push_back(v2);

        int m_roiStart = HydrusParameters::roiStart;
        // draw the data
        circle(img_draw, cen,24 / m_para.binNum, Scalar(0, 0, 255), -1);
        circle(img_draw, h1, 24 / m_para.binNum, Scalar(0, 0, 255), -1);
        circle(img_draw, h2, 24 / m_para.binNum, Scalar(0, 0, 255), -1);
        circle(img_draw, v1, 24 / m_para.binNum, Scalar(0, 0, 255), -1);
        circle(img_draw, v2, 24 / m_para.binNum, Scalar(0, 0, 255), -1);
        // dayin shuju 
        string xstr = to_string(h1.x);
        string ystr = to_string(h1.y);
        string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
        cv::putText(img_draw, text, h1, FONT_HERSHEY_PLAIN,2,Scalar(255,0,255),2);
        xstr = to_string(h2.x);
        ystr = to_string(h2.y);
        text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
        cv::putText(img_draw, text, h2, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 2);
        xstr = to_string(v1.x);
        ystr = to_string(v1.y);
        text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
        cv::putText(img_draw, text, v1, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 2);
        xstr = to_string(v2.x);
        ystr = to_string(v2.y);
        text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
        cv::putText(img_draw, text, v2, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 2);

        
        //calculate the FOV
       // cen = HydrusParameters::W_ND0_opticalcenter / hydrusConfig.m_para.binNum;
        double disH = Getdistance(h1, cen);
        double disH1 = Getdistance(h2, cen);
        double disV = Getdistance(v1, cen);
        double disV1 = Getdistance(v2, cen);
        double m_FocalLength = HydrusParameters::FocalLength;
        double m_pixel_size = m_para.pixel_size;
        double FovH = atan(m_pixel_size * disH / m_FocalLength) * 180.0 / CV_PI;
        double FovH1 = atan(m_pixel_size * disH1 / m_FocalLength) * 180.0 / CV_PI;
        double FovV = atan(m_pixel_size * disV / m_FocalLength) * 180.0 / CV_PI;
        double FovV1 = atan(m_pixel_size * disV1 / m_FocalLength) * 180.0 / CV_PI;
       
        re.FovH = FovH + FovH1;
        re.FovV = FovV + FovV1;
        m_fov = re;
        m_img_draw = img_draw.clone();
    }
    return re;
}
FovAngleRe algorithm::MLCrosshairPara::FOVAngleCross(cv::Mat img)
{
    if (img.data != NULL)
    {
        // GetNewPara(img);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        img = getRotationImg(img, m_rotationAngle);
        int bin = m_para.binNum;
        cv::Rect rec, rect;
        cv::Mat img8 = convertToUint8(img);
        cv::Mat roi1 = getRectROIImg(img, m_para.rect); // img8(m_para.rect).clone();
        rec = findborder(roi1);
        if (rec.x != 0 || rec.y != 0)
        {
            rect.x = rec.x + m_para.rect.x;
            rect.y = rec.y + m_para.rect.y;
            rect.width = rec.width;
            rect.height = rec.height;
        }
        else
            rect = m_para.rect;
        m_fov.roiRect = rect;
        int m_roiStart = HydrusParameters::roiStart;
        cv::Mat img_draw = img8.clone();
        cv::Mat roi = GetROIMat(img8, rect);
        int row = roi.rows;
        int col = roi.cols;
        vector<cv::Rect> rectVec;
        // vector<cv::Mat> splitImg = ImageSegmentationTemplate1(roi, rectVec); // ImageSegmentation(roi);
        vector<cv::Mat> splitImg = ImageSegmentation(roi, rectVec);

        cv::Point2f center, realcenter;
        vector<double> Vline, Hline;
        Point end;
        vector<Point2f> imgend;
        if (splitImg.size() == 9)
        {
            for (int i = 1; i < splitImg.size(); i = i + 2)
            {
                cv::Mat tmp = splitImg[i];
                cv::Rect rectMat = rectVec[i];
                //  rectMat.x = rectMat.x - 60;
                //  rectMat.y = rectMat.y - 60;
                //  rectMat.width = rectMat.width + 120;
                //  rectMat.height = rectMat.height + 120;
                //  cv::Rect roiRect = cv::Rect(0, 0, roi.cols, roi.rows);
                //  cv::Rect andRect = rectMat & roiRect;
                //  // add
                //  //TODO:
                ////  tmp = roi(andRect);
                CrossCenter cc(tmp, 10);
                int flag = HydrusParameters::crosshairAlgortihm;
                //  center = getendpointHoughLine(tmp,i); // getendpointTemplate(tmp, i); // cc.get_crossCenter(flag);
                center = getendpointTest(tmp);
                if (center.x >= 0 && center.y >= 0)
                {
                    realcenter.x = center.x + rectMat.x + m_roiStart + rect.x;
                    realcenter.y = center.y + rectMat.y + m_roiStart + rect.y;
                    circle(img_draw, realcenter, 2, Scalar(255, 0, 255), -1);
                    imgend.push_back(realcenter);
                }
            }
            cv::Point2f cen;
            // update,need to calculate in real time
            // GetBoresight();
            CrossCenter cc1(splitImg[4], 10);
            cen = cc1.find_centerGaussian(splitImg[4]);
            cen.x = cen.x + rectVec[4].x + m_roiStart + rect.x;
            cen.y = cen.y + rectVec[4].y + m_roiStart + rect.y;

            // if (m_center.x != 0 && m_center.y != 0)
            //{
            //    cen.x = m_center.x / bin;
            //    cen.y = m_center.y / bin;
            //}
            // else {
            //    GetBoresight();
            //    cen.x = m_center.x / bin;
            //    cen.y = m_center.y / bin;
            //}

            if (imgend.size() == 4 && cen.x > 0 && cen.y > 0)
            {
                for (int i = 0; i < imgend.size(); i++)
                {
                    circle(img_draw, imgend[i], 2, Scalar(255, 0, 255), -1);
                }
                double disH = Getdistance(imgend[0], cen);
                double disH1 = Getdistance(imgend[3], cen);
                double disV = Getdistance(imgend[1], cen);
                double disV1 = Getdistance(imgend[2], cen);
                circle(img_draw, cen, 2, Scalar(255, 0, 255), -1);
                double m_FocalLength = HydrusParameters::FocalLength;
                double m_pixel_size = m_para.pixel_size;
                double FovH = atan(m_pixel_size * disH / m_FocalLength) * 180.0 / CV_PI;
                double FovH1 = atan(m_pixel_size * disH1 / m_FocalLength) * 180.0 / CV_PI;
                double FovV = atan(m_pixel_size * disV / m_FocalLength) * 180.0 / CV_PI;
                double FovV1 = atan(m_pixel_size * disV1 / m_FocalLength) * 180.0 / CV_PI;
                m_fov.FovH = FovH + FovH1;
                m_fov.FovV = FovV + FovV1;
                m_fov.EdgePoint = imgend;
                m_img_draw = img_draw.clone();
            }
        }
    }
    return m_fov;
}

bool algorithm::MLCrosshairPara::findEdgePts(cv::Mat data, int flag, cv::Point2f &p1, cv::Point2f &p2)
{
    if (flag == 1)
        cv::rotate(data, data, ROTATE_90_COUNTERCLOCKWISE);
    cv::Mat gx, gy;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 1));
    cv::morphologyEx(data, gx, cv::MORPH_GRADIENT, element);
    double maxV;
    cv::minMaxLoc(gx,NULL,&maxV);
    vector<int> xVec = findPeaks(gx, maxV/3.0,0,0);
    if (xVec.size() != 2)
        return false;

    if (flag==0)
    {
        p1.x = xVec[0];
        p2.x = xVec[1];
    }
    else if(flag==1)
    {
        p1.y = xVec[0];
        p2.y = xVec[1];
    }
    return true;
}

vector<int> algorithm::MLCrosshairPara::findPeaks(cv::Mat data, double minHeight, int minWidth, double minProminence)
{
    vector<int> xVec;
   // peaks.clear(); // 清空峰值数组
    data.convertTo(data, CV_64FC1);
    int thresh = 1;
    for (int i = thresh; i < data.cols - thresh; ++i)
    {
        // 判断是否为峰值，即比相邻的两个点大，并且大于设定的最小高度
        // for (int i0 = i - thresh;  i0 < i + thresh; i0++)
        //{
        //        if (data.at<double>(0, i) > data.at<double>(0, i0));
        //        break;
        //}
        if (data.at<double>(0, i) > minHeight &&
            (data.at<double>(0, i) > data.at<double>(0, i - 1) && data.at<double>(0, i) >= data.at<double>(0, i + 1)))
        {
            // 寻找峰值的宽度，即峰值到其两侧的极小值点的距离
            int left = i - thresh;
            while (left >= 0 && (data.at<double>(0, left) <= data.at<double>(0, left + thresh)))
            {
                --left;
            }

            int right = i + thresh;
            while (right < data.cols && data.at<double>(0, right) <= data.at<double>(0, right - thresh))
            {
                ++right;
            }

            // 计算峰值的宽度和突出度
            int width = right - left - 2;
            double prominence =
                data.at<double>(0, i) - std::max(data.at<double>(0, left + 1), data.at<double>(0, right - 1));

            // 判断峰值的宽度和突出度是否满足设定的条件
            if (width >= minWidth && prominence > minProminence)
            {
                if (xVec.size() > 0)
                {
                    int sub = i - xVec[xVec.size() - 1];
                    if (sub > 100)
                    {
                       // peaks.push_back({i, data.at<double>(0, i), width, prominence});
                        xVec.push_back(i);
                    }
                }
                else
                {
                   // peaks.push_back({i, data.at<double>(0, i), width, prominence});
                    xVec.push_back(i);
                }
            }
        }
    }
    return xVec;
}

double algorithm::MLCrosshairPara::crossRotation(cv::Mat img)
{
    double rotation = -1;
    Point2f cenval1;
    Point2f cenval7;
    vector<cv::Point2f> cenVec;

    if (img.data != NULL)
    {
        // GetNewPara(img);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
        img = getRotationImg(img, m_rotationAngle);
        cv::Rect rec, rect;
        cv::Mat img8 = convertToUint8(img);
        cv::Mat roi1 = getRectROIImg(img, m_para.rect);
        rec = findborder(roi1);
        cv::Mat img_draw = convertToUint8(img);
        if (rec.x != 0 || rec.y != 0)
        {
            rect.x = rec.x + m_para.rect.x;
            rect.y = rec.y + m_para.rect.y;
            rect.width = rec.width;
            rect.height = rec.height;
        }
        else
            rect = m_para.rect;
        int m_roiStart = HydrusParameters::roiStart;
        cv::Mat roi = GetROIMat(img8, rect);
        // TODO::test confirm the file transform
        int row = roi.rows;
        int col = roi.cols;
        vector<cv::Rect> rectVec;
        //        vector<cv::Mat> splitImg = ImageSegmentationTemplate1(roi, rectVec);
        vector<cv::Mat> splitImg = ImageSegmentation(roi, rectVec);

        if (splitImg.size() == 9)
        {
            cv::Mat roi1 = splitImg[1];
            cv::Rect rectMat = rectVec[1];
            CrossCenter cc(roi1, 100);
            cc.SetbinNum(m_para.binNum);
            int flag = HydrusParameters::crosshairAlgortihm;
            cv::Point2f c1 = cc.find_centerGradAndGaussian(roi1);
            if (c1.x <= 0 || c1.y <= 0)
                c1 = cc.find_centerHoughLineAndWidth(roi1, 1, -1);

            if (c1.x != 0 && c1.y != 0)
            {
                cenval1.x = c1.x + rectMat.x + m_roiStart + rect.x;
                cenval1.y = c1.y + rectMat.y + m_roiStart + rect.y;
                cv::Mat roi2 = splitImg[7];
                rectMat = rectVec[7];
                CrossCenter dd(roi2, 100);
                dd.SetbinNum(m_para.binNum);
                cv::Point2f c2 = dd.find_centerGradAndGaussian(roi2);
                if (c2.x <= 0 || c2.y <= 0)
                    c2 = cc.find_centerHoughLineAndWidth(roi2, 7, -1);

                if (c2.x != 0 && c2.y != 0)
                {
                    cenval7.x = c2.x + rectMat.x + m_roiStart + rect.x;
                    cenval7.y = c2.y + rectMat.y + m_roiStart + rect.y;
                    cv::circle(img_draw, cenval1, 10, Scalar(255, 0, 255), -1);
                    cv::circle(img_draw, cenval7, 10, Scalar(255, 0, 255), -1);
                    double deltaPixelX = cenval7.x - cenval1.x;
                    double deltaPixelY = cenval7.y - cenval1.y;
                    rotation = atan(deltaPixelY / deltaPixelX) * 180.0 / CV_PI;
                    // TODO: add image to watch
                }
            }
        }
        m_img_draw = img_draw.clone();
    }
    return rotation;
}

double algorithm::MLCrosshairPara::crossRotationNoBorder(cv::Mat img)
{
    double rotation = -1;
    if (img.data != NULL)
    {
        m_img_draw.release();
        // GetNewPara(img);
        double rotationAngle = HydrusParameters::roationAngle;
        img = getRotationImg(img, rotationAngle);
       // img = getRotationImg(img, m_rotationAngle);
        cv::Rect rec, rect;
        cv::Mat img8 = convertToUint8(img);
        //cv::Mat roi1 = img8(m_para.rect);
        cv::Mat img_draw = convertTo3Channels(img8);
        MultiCrossHairDetection md;
        vector<cv::Point2f> ptsVec;
        MultiCrossHairRe cenRe = md.getMultiCrossHairCenterBySlope(img8, ptsVec);
        if (cenRe.flag.success)
        {
            cv::Mat lMat = img8(cenRe.rectMap["ML"]).clone();
            cv::Mat rMat = img8(cenRe.rectMap["MR"]).clone();
            CrossCenter cc;
            cv::Point2f lcen, rcen;
            if (isSLB)
            {
                 lcen = cc.find_centerLINES(lMat);
                  rcen = cc.find_centerLINES(rMat);
            }else
            {
                 lcen = cc.find_centerGaussian(lMat);
                 rcen = cc.find_centerGaussian(rMat);
            }

            if (lcen.x > 1e-6 && lcen.y > 1e-6 && rcen.x > 1e-6 && rcen.y > 1e-6)
            {
                lcen = lcen + Point2f(cenRe.rectMap["ML"].x, cenRe.rectMap["ML"].y);
                rcen = rcen + Point2f(cenRe.rectMap["MR"].x, cenRe.rectMap["MR"].y);
                m_rotaionPts.clear();
                m_rotaionPts.push_back(lcen);
                m_rotaionPts.push_back(rcen);
                circle(img_draw, lcen, 16/m_para.binNum, Scalar(0, 0, 255), -1);
                circle(img_draw, rcen, 16 / m_para.binNum, Scalar(0, 0, 255), -1);

                string xstr = to_string(lcen.x);
                string ystr = to_string(lcen.y);
                string text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);
                cv::putText(img_draw, text, lcen, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 2);
                 xstr = to_string(rcen.x);
                 ystr = to_string(rcen.y);
                 text = xstr.substr(0, xstr.size() - 4) + "," + ystr.substr(0, ystr.size() - 4);

                cv::putText(img_draw, text, rcen, FONT_HERSHEY_PLAIN, 2, Scalar(255, 0, 255), 2);

                double deltaPixelX = lcen.x - rcen.x;
                double deltaPixelY = lcen.y - rcen.y;
                rotation = atan(deltaPixelY / deltaPixelX) * 180.0 / CV_PI;
                m_rotationAngle = rotation;
            }
        }
        
        m_img_draw = img_draw.clone();
    }
    return rotation;
}

LaterColorRe algorithm::MLCrosshairPara::LateralColor(imgRGB img)
{

    cv::Mat RGmat, BGmat, RBmat;
    double avgRG = 0, avgBG = 0, avgRB = 0;
    if (img.imgR.data != NULL && img.imgG.data != NULL && img.imgB.data != NULL)
    {
        double rotationAngle = HydrusParameters::roationAngle;
        img.imgR = getRotationImg(img.imgR, rotationAngle);
        img.imgR = getRotationImg(img.imgR, rotationAngle);
        img.imgB = getRotationImg(img.imgB, rotationAngle);

        // GetNewPara(img.imgR);
        cv::Rect recR, recG, recB;
        cv::Mat roiR, roiG, roiB;
        recR = findborder(img.imgR);
        recG = findborder(img.imgG);
        recB = findborder(img.imgB);
        if (recR.x != 0 && recR.y != 0 && recG.x != 0 && recG.y != 0 && recB.x != 0 && recB.y != 0)
        {
            recR = recR;
            recG = recG;
            recB = recB;
        }
        else
        {
            recR = m_para.rect;
            recG = m_para.rect;
            recB = m_para.rect;
        }
        vector<cv::Point2f> cenVecR, cenVecG, cenVecB;
        cenVecR = Calculatecrosscenter(img.imgR, recR);
        m_imgR_draw = m_img_draw.clone();
        cenVecG = Calculatecrosscenter(img.imgG, recG);
        m_imgG_draw = m_img_draw.clone();
        cenVecB = Calculatecrosscenter(img.imgB, recB);
        m_imgB_draw = m_img_draw.clone();
        vector<double> disRG, disBG, disRB;
        double avgRG, avgBG, avgRB, maxRG, maxBG, maxRB;
        RGmat = CalculateCrosshairAngDis(cenVecR, cenVecG, avgRG, maxRG);
        BGmat = CalculateCrosshairAngDis(cenVecB, cenVecG, avgBG, maxBG);
        RBmat = CalculateCrosshairAngDis(cenVecR, cenVecB, avgRB, maxRB);
        m_laterColarRe.BGmat = BGmat;
        m_laterColarRe.RBmat = RBmat;
        m_laterColarRe.RGmat = RGmat;
        m_laterColarRe.avgBG = avgBG;
        m_laterColarRe.avgRB = avgRB;
        m_laterColarRe.avgRG = avgRG;
        m_laterColarRe.maxBG = maxBG;
        m_laterColarRe.maxRB = maxRB;
        m_laterColarRe.maxRG = maxRG;
    }
    return m_laterColarRe;
}

std::vector<cv::Mat> algorithm::MLCrosshairPara::ImageSegmentation(cv::Mat img, vector<cv::Rect> &rectVec)
{
    if (img.data != NULL)
    {
        cv::Mat img_draw = img.clone();
        int start = HydrusParameters::roiStart; // m_roiStart;
        vector<cv::Mat> splitImg;
        int m = img.rows;
        int n = img.cols;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                cv::Rect rect = Rect(n / 3 * i + start, m / 3 * j + start, n / 3 - 2 * start, m / 3 - 2 * start);
                // draw
                cv::rectangle(img_draw, rect, cv::Scalar(255, 0, 255), 3);
                cv::Rect rectROI = Rect(n / 3 * i + start, m / 3 * j + start, n / 3 - 2 * start, m / 3 - 2 * start);
                cv::Mat roi = img(rectROI);
                rectVec.push_back(rectROI);
                splitImg.push_back(roi);
            }
        }
        return splitImg;
    }
}
std::vector<cv::Mat> algorithm::MLCrosshairPara::ImageSegmentationTemplate(cv::Mat img, vector<cv::Rect> &rectVec)
{
    if (img.data != NULL)
    {
        cv::Mat img_draw = img.clone();
        int start = HydrusParameters::roiStart; // m_roiStart;
        vector<cv::Mat> splitImg, splitImgNew;
        cv::Mat templ = cv::imread("./config/templateImg/template41.tif");
        // cv::Mat templ = cv::imread("./config/templateImg/template4.tif");
        vector<cv::Point> P;
        GetAllMinLoc(img, templ, 0.3, Scalar(0, 0, 0), &P, 9);
        vector<cv::Point> pts, ptsNew;
        pts = convecHull(P);
        // pts clusters
        vector<double> ptdis;
        cv::Mat ptsMap = cv::Mat::zeros(cv::Size(1, 9), CV_32F);
        for (int i = 0; i < P.size(); i++)
        {
            double dis = getDistancePL(pts[0], pts[pts.size() - 1], P[i]);
            ptdis.push_back(dis);
            ptsMap.at<float>(i, 0) = dis;
        }
        double min = *min_element(ptdis.begin(), ptdis.end());
        double max = *max_element(ptdis.begin(), ptdis.end());
        Mat labelB1;
        Mat centersB1;
        TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 0.1);
        cv::kmeans(ptsMap, 3, labelB1, criteria, 1, KMEANS_PP_CENTERS, centersB1);
        vector<int> index1, index2, index3;
        vector<int> index11, index21, index31;
        vector<double> xVec1, xVec2, xVec3;
        vector<double> xVec11, xVec21, xVec31;

        for (int i = 0; i < 9; i++)
        {
            int tmp = labelB1.at<int>(i, 0);
            if (tmp == 0)
            {
                index1.push_back(i);
                xVec1.push_back(P[i].x);
            }
            else if (tmp == 2)
            {
                index3.push_back(i);
                xVec3.push_back(P[i].x);
            }
            else if (tmp == 1)
            {
                index2.push_back(i);
                xVec2.push_back(P[i].x);
            }
        }

        vector<int> index;
        index = sortIndex(xVec1);
        ptsNew.push_back(P[index1[index[0]]]);
        ptsNew.push_back(P[index1[index[1]]]);
        ptsNew.push_back(P[index1[index[2]]]);

        index.clear();
        index = sortIndex(xVec2);
        ptsNew.push_back(P[index2[index[0]]]);
        ptsNew.push_back(P[index2[index[1]]]);
        ptsNew.push_back(P[index2[index[2]]]);

        index.clear();
        index = sortIndex(xVec3);
        ptsNew.push_back(P[index3[index[0]]]);
        ptsNew.push_back(P[index3[index[1]]]);
        ptsNew.push_back(P[index3[index[2]]]);
        vector<cv::Rect> rectVec1;
        for (int i = 0; i < ptsNew.size(); i++)
        {
            cv::Rect rect = cv::Rect(ptsNew[i].x, ptsNew[i].y, templ.rows, templ.cols);
            cv::Mat roi = img(rect);
            splitImg.push_back(roi);
            rectangle(img_draw, rect, Scalar(255, 0, 255), 1);
            rectVec1.push_back(rect);
        }
        splitImgNew.push_back(splitImg[6]);
        splitImgNew.push_back(splitImg[3]);
        splitImgNew.push_back(splitImg[0]);
        splitImgNew.push_back(splitImg[7]);
        splitImgNew.push_back(splitImg[4]);
        splitImgNew.push_back(splitImg[1]);
        splitImgNew.push_back(splitImg[8]);
        splitImgNew.push_back(splitImg[5]);
        splitImgNew.push_back(splitImg[2]);

        rectVec.push_back(rectVec1[6]);
        rectVec.push_back(rectVec1[3]);
        rectVec.push_back(rectVec1[0]);
        rectVec.push_back(rectVec1[7]);
        rectVec.push_back(rectVec1[4]);
        rectVec.push_back(rectVec1[1]);
        rectVec.push_back(rectVec1[8]);
        rectVec.push_back(rectVec1[5]);
        rectVec.push_back(rectVec1[2]);

        return splitImgNew;
    }
}
std::vector<cv::Mat> algorithm::MLCrosshairPara::ImageSegmentationTemplate1(cv::Mat img, vector<cv::Rect> &rectVec)
{
    if (img.data != NULL)
    {
        cv::Mat img_draw = img.clone();
        int start = HydrusParameters::roiStart; // m_roiStart;
        vector<cv::Mat> splitImg, splitImgNew;
        cv::Mat templ;
        if (DEBUG == 1)
        {
            templ = cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\templateImg\\template4.tif");
        }
        else
            templ = cv::imread("./config/templateImg/template3.tif");
        vector<cv::Point> P;
        GetAllMinLoc(img, templ, 0.3, Scalar(0, 0, 0), &P, 9);
        vector<cv::Point> pts;
        pts = convecHull(P);
        vector<double> yVec;
        for (int i = 0; i < pts.size(); i++)
        {
            yVec.push_back(pts[i].y);
        }
        double max = *max_element(yVec.begin(), yVec.end());
        double ySub = abs(max - pts[0].y);
        vector<cv::Point> ptsNew;
        if (ySub > 500)
        {
            for (int i = 2; i < pts.size(); i++)
            {
                ptsNew.push_back(pts[i]);
            }
            ptsNew.push_back(pts[0]);
            ptsNew.push_back(pts[1]);
            pts.clear();
            pts = ptsNew;
        }

        // pts clusters
        vector<double> ptdis;
        for (int i = 0; i < P.size(); i++)
        {
            double dis = getDistancePL(pts[0], pts[pts.size() - 1], P[i]);
            ptdis.push_back(dis);
        }
        vector<double> disUnique;
        vector<int> indexVec;
        cv::Mat indexMap;
        disUnique = dataIndexMap(ptdis, P, indexMap, 50);
        int sum = indexMap.rows * indexMap.cols;
        vector<cv::Rect> rectVec1;
        if (sum == P.size())
        {
            for (int i = 0; i < indexMap.rows; i++)
            {
                for (int j = 0; j < indexMap.cols; j++)
                {
                    int in = indexMap.at<uchar>(j, i);
                    cv::Rect rect = cv::Rect(P[in].x, P[in].y, templ.rows, templ.cols);
                    cv::Mat roi = img(rect);
                    splitImg.push_back(roi);
                    rectangle(img_draw, rect, Scalar(255, 0, 255), 1);
                    rectVec1.push_back(rect);
                }
            }
        }
        splitImgNew = splitImg;
        rectVec = rectVec1;
        return splitImgNew;
    }
}
cv::Rect algorithm::MLCrosshairPara::findborder(cv::Mat img, cv::Rect rect)
{
    cv::Rect rect_new = Rect(0, 0, -1, -1);
    // cv::Mat img_draw = img.clone();
    if (img.data != NULL)
    {
        cv::Mat img_gray, img_gray1, img_draw;
        img_gray = GetROIMat(img, rect);
        if (img_gray.depth() == CV_8U)
        {
            img_gray1 = img_gray.clone();
            img_draw = img.clone();
        }
        else
        {
            img_gray1 = convertToUint8(img_gray);
            img_draw = convertToUint8(img);
        }
        std::vector<std::vector<cv::Point>> contours, contoursNew;
        std::vector<cv::Vec4i> hierarchy;
        cv::Mat img_process, srcbinary;
        cv::Mat element1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
        cv::GaussianBlur(img_gray1, img_gray1, cv::Size(9, 9), 2, 2);
        cv::morphologyEx(img_gray1, img_gray1, cv::MORPH_CLOSE, element1);
        cv::morphologyEx(img_gray1, img_gray1, cv::MORPH_GRADIENT, element1);
        threshold(img_gray1, srcbinary, 0, 255, THRESH_OTSU);
        NaiveRemoveNoise(srcbinary, 500);
        // cv::Mat img_canny;
        // cv::Canny(srcbinary, img_canny, 50, 150);
        cv::findContours(srcbinary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
        vector<double> area_sort;
        vector<int> list;
        cv::Mat roi;
        vector<double> areaVec;
        for (int i = 0; i < contours.size(); ++i)
        {
            double area = contourArea(contours[i], false);
            // cout << area << endl;
            double powbin = pow(m_para.binNum, 2);
            if (area > 4e4 / powbin) //&& area < 4.2e6 / powbin)
            // if (area > 4e6 / powbin && area < 6e6 / powbin)
            {
                rect_new = cv::boundingRect(cv::Mat(contours[i]));
                cv::rectangle(img_draw, rect_new, Scalar(255, 0, 255), 1);
                contoursNew.push_back(contours[i]);
                areaVec.push_back(area);
                // roi = img_gray1(rect_new);
            }
        }
        if (contoursNew.size() > 0)
        {
            int maxPos = max_element(areaVec.begin(), areaVec.end()) - areaVec.begin();
            rect_new = cv::boundingRect(cv::Mat(contoursNew[maxPos]));
            roi = img_draw(rect_new);
        }
        // if (areamax.size() != NULL)
        //{
        //    sort_user(areamax, area_sort, list); // change sort by x,from top to bottom
        //    rect_new = cv::boundingRect(cv::Mat(contours[list[0]]));
        //    cv::rectangle(img_draw, rect_new, Scalar(255, 0, 255), 1);
        //    cv::Mat roi = img(rect_new);
        //}
    }
    return rect_new;
}

cv::Rect algorithm::MLCrosshairPara::findborderHoughLine(cv::Mat img, cv::Rect rect)
{
    return cv::Rect();
}

cv::Point algorithm::MLCrosshairPara::getendpointTemplate(cv::Mat img, int i)
{
    cv::Point re = cv::Point(0, 0);
    if (img.data != NULL)
    {
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = img8.clone();
        cv::Point2f c0; //=find_centerGrad(img8);
        if (abs(c0.x) < 1e-6 || abs(c0.y) < 1e-6)
        {
            // cv::Mat tmpl = cv::imread("E:\\project\\mtfiq\\src\\app\\config\\templateImg\\template3.tif");
            // cv::Mat tmpl = cv::imread("D:\\GitProject\\Hydrus_2\\src\\app\\config\\templateImg\\template3.tif");
            cv::Mat tmpl;
            if (DEBUG == 1)
            {
                tmpl = cv::imread("E:\\project\\hydrus2\\mtfiq\\src\\app\\config\\templateImg\\template3.tif");
            }
            else
            {
                tmpl = cv::imread("./config/templateImg/template3.tif");
            }
            CrossCenter cc; // (img8, 10);
            c0 = cc.find_centerTemplate(img8, tmpl);
        }
        if (abs(c0.x) > 1e-6 & abs(c0.y) > 1e-6)
        {
            int width = 300 / m_para.binNum;
            cv::Point start, end;
            start.x = max(int(c0.x - width / 2.0), 0);
            start.y = max(int(c0.y - width / 2.0), 0);
            end.x = min(int(c0.x + width / 2.0), img8.cols);
            end.y = min(int(c0.y + width / 2.0), img8.rows);
            cv::Rect rect = cv::Rect(start, end);
            cv::Mat roi = img8(rect);

            CrossCenter cc(roi, 10);
            cv::Point center = cc.get_crossCenter(1); // use the houghline
            double linewidth = cc.get_line_width();
            center = center + cv::Point(rect.x, rect.y);
            switch (i)
            {
            case 1: {
                re.x = center.x - linewidth / 2.0;
                re.y = center.y;
                break;
            }
            case 7: {
                re.x = center.x + linewidth / 2.0;
                re.y = center.y;
                break;
            }
            case 3: {
                re.x = center.x;
                re.y = center.y - linewidth / 2.0;
                break;
            }
            case 5: {
                re.x = center.x;
                re.y = center.y + linewidth / 2.0;
                break;
            }
            }
            circle(img_draw, re, 5, cv::Scalar(255, 0, 255), -1);
        }
    }
    return re;
}

cv::Point2f algorithm::MLCrosshairPara::getendpointTest(cv::Mat img)
{
    cv::Point re = cv::Point(0, 0);
    if (img.data != NULL)
    {
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = img8.clone();
        cv::Mat img_gray;
        if (img.channels() == 3)
            cvtColor(img, img_gray, COLOR_BGR2GRAY);
        else
            img_gray = img.clone();
        cv::GaussianBlur(img_gray, img_gray, Size(5, 5), 0, 0);

        // re=findPointEnd(img);
        // CrossCenter cc;
        // re=cc.find_centerGaussian(img);
        cv::Mat row0, col0;
        reduce(img, row0, 0, CV_REDUCE_AVG);
        reduce(img, col0, 1, CV_REDUCE_AVG);
        double min = 0, max = 0;
        cv::Point minloc, maxloc, maxloc1;
        minMaxLoc(row0, &min, &max, &minloc, &maxloc);
        minMaxLoc(col0, &min, &max, &minloc, &maxloc1);

        re = cv::Point2f(maxloc.x, maxloc1.y);

        circle(img_draw, re, 1, Scalar(255, 0, 255), -1);
    }
    return re;
}

cv::Point2f algorithm::MLCrosshairPara::findPointEnd(cv::Mat img)
{

    cv::Point2f c0{0, 0};
    cv::Mat img_draw;
    cv::Mat img_gray1, img_gray;
    if (img.channels() == 3)
        cvtColor(img, img_gray1, CV_BGR2GRAY);
    else
        img_gray1 = img.clone();

    if (img_gray1.depth() == CV_8U)
    {
        img_gray = img_gray1.clone();
        img_draw = img.clone();
    }
    else
    {
        img_gray = convertToUint8(img_gray1);
        img_draw = convertToUint8(img);
    }
    cv::medianBlur(img_gray, img_gray, 3);
    cv::Scalar meanAll;
    meanAll = cv::mean(img_gray);
    double min = 0, max = 0;
    cv::Point minloc, maxloc;
    minMaxLoc(img_gray, &min, &max, &minloc, &maxloc);
    int left = cv::max(maxloc.x - 10, 0);
    int right = cv::min(maxloc.x + 10, img_gray.cols);
    int bottom = cv::max(maxloc.y - 10, 0);
    int top = cv::min(maxloc.y + 10, img_gray.rows);
    cv::Rect rec = cv::Rect(left, bottom, right - left, top - bottom);
    cv::Mat roi = img_gray(rec); // img_gray(cv::Range(left,right), cv::Range(bottom,top));
    cv::Scalar meanRoi = cv::mean(roi);
    if (meanRoi(0) > meanAll(0))
    {
        cv::Mat roi = img_gray.clone();
        // GaussianBlur(roi, roi, Size(5, 5), 5, 5);
        double minVal;
        double maxVal;
        Point minLoc;
        Point maxLoc;
        // grad
        cv::Mat xG, yG;
        // Mat element1 = getStructuringElement(MORPH_RECT, Size(size, size));
        // morphologyEx(img_gray, img_gray, MORPH_CLOSE, element1);
        cv::Sobel(roi, xG, CV_32FC1, 1, 0, 3);
        cv::Sobel(roi, yG, CV_32FC1, 0, 1, 3);
        convertScaleAbs(xG, xG); //
        convertScaleAbs(yG, yG);
        cv::Mat sum = xG + yG;
        cv::Mat sobelxy;
        cv::addWeighted(xG, 0.5, yG, 0.5, 0, sobelxy);
        cv::Mat xGth, yGth;
        cv::threshold(xG, xGth, 0, 255, CV_THRESH_OTSU);
        cv::threshold(yG, yGth, 0, 255, CV_THRESH_OTSU);
        Clear_MicroConnected_Areas(xGth, xGth, 30);
        Clear_MicroConnected_Areas(yGth, yGth, 30);
        cv::Mat xGcan, yGcan;
        cv::Canny(xGth, xGcan, 30, 90);
        cv::Canny(yGth, yGcan, 30, 90);
        // NaiveRemoveNoise();
        vector<cv::Point2f> centers;
        for (int i = 1; i < xGcan.rows; i++)
        {
            for (int j = 0; j < xGcan.cols; j++)
            {
                int x = xGcan.at<uchar>(i, j);
                int y = yGcan.at<uchar>(i, j);
                if (x == 255 & y == 255)
                {
                    centers.push_back(cv::Point2f(j, i));
                    cv::circle(roi, cv::Point2f(j, i), 3, cv::Scalar(255, 0, 255), -1);
                    // cout << cv::Point2f(j, i) << endl;
                }
            }
        }
        vector<double> dis1;
        double k[50] = {0};
        cv::Mat kMat = cv::Mat::zeros(cv::Size(1, 50), CV_8UC1);
        cv::Mat dis = cv::Mat::zeros(cv::Size(50, 50), CV_32FC1);
        cv::Mat index = cv::Mat::zeros(cv::Size(50, 50), CV_8UC1);
        if (centers.size() > 0 & centers.size() < 50)
        {
            double tmp = Getdistance(centers[0], cv::Point2f(0, 0));
            dis.at<float>(0, k[0]) = tmp;
            index.at<uchar>(0, 0) = 0;
            k[0]++;
            kMat.at<uchar>(0, 0) = k[0];
            for (int i = 1; i < centers.size(); i++)
            {
                double tmp = Getdistance(centers[i], cv::Point2f(0, 0));
                int flag = 0;
                int j = 0;
                for (j = 0; j < dis.rows; j++)
                {
                    if (dis.at<float>(j, 0) > 0)
                    {
                        double d = abs(tmp - dis.at<float>(j, 0));
                        int in = index.at<uchar>(j, 0);
                        double dx = abs(centers[i].x - centers[index.at<uchar>(j, 0)].x);
                        double dy = abs(centers[i].y - centers[index.at<uchar>(j, 0)].y);
                        int distance = CrossCenterParameter::pointdistance;
                        if (dx < distance && dy < distance)
                        {
                            dis.at<float>(j, k[j]) = tmp;
                            index.at<uchar>(j, k[j]) = i;
                            k[j]++;
                            kMat.at<uchar>(j, 0) = k[j];
                            flag = 1;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (flag == 0)
                {
                    dis.at<float>(j, k[j]) = tmp;
                    index.at<uchar>(j, k[j]) = i;
                    k[j]++;
                    kMat.at<uchar>(j, 0) = k[j];
                }
            }
            kMat.setTo(255, kMat == 0);
            cv::minMaxLoc(kMat, &minVal, &maxVal, &minLoc, &maxLoc);

            //
            int num = minVal;
            int in = minLoc.y;

            // cout << "test" << endl;
            double sumx = 0, sumy = 0;
            for (int i = 0; i < num; i++)
            {
                int in1 = index.at<uchar>(in, i);
                sumx = sumx + centers[in1].x;
                sumy = sumy + centers[in1].y;
                //  cout << centers[in1] << endl;
            }
            c0 = cv::Point2f(sumx / num, sumy / num);
            // c0.x = c0.x + maxloc.x;
            //  c0.y = c0.y + maxloc.y;
            circle(img_draw, c0, 1, cv::Scalar(255, 0, 255), -1);
        }
    }
    return c0;
}

cv::Point algorithm::MLCrosshairPara::getendpointHoughLine(cv::Mat img, int i)
{

    cv::Point re = cv::Point(0, 0);
    cv::Point cend;
    if (img.data != NULL)
    {
        cv::Mat img8 = convertToUint8(img);
        cv::Mat img_draw = img8.clone();
        CrossCenter a;
        //  cv::Point c0 = a.find_centerGrad(img8);
        //   if (abs(c0.x) > 1e-6 & abs(c0.y) > 1e-6)
        // {
        CrossCenter cc(img, 10);
        cv::Point center = cc.get_crossCenter(1); // use the houghline
        double linewidth = cc.get_line_width();
        switch (i)
        {
        case 1: {
            re.x = center.x - linewidth / 2.0;
            re.y = center.y;
            break;
        }
        case 7: {
            re.x = center.x + linewidth / 2.0;
            re.y = center.y;
            break;
        }
        case 3: {
            re.x = center.x;
            re.y = center.y - linewidth / 2.0;
            break;
        }
        case 5: {
            re.x = center.x;
            re.y = center.y + linewidth / 2.0;
            break;
        }
        }
        circle(img_draw, re, 1, cv::Scalar(255, 0, 255), -1);
        //   }
    }
    return re;
}

std::vector<cv::Point2f> algorithm::MLCrosshairPara::Calculatecrosscenter(cv::Mat img, cv::Rect rect)
{
    vector<cv::Point2f> cenVec, NewcenVec;
    if (img.data != NULL)
    {
        int m_roiStart = HydrusParameters::roiStart;
        cv::Mat img_draw;
        img_draw = convertToUint8(img);
        cv::Mat roi = GetROIMat(img_draw, rect);
        vector<cv::Rect> rectVec;
        vector<cv::Mat> splitImg = ImageSegmentation(roi, rectVec);
        int row = roi.rows;
        int col = roi.cols;
        for (int i = 0; i < splitImg.size(); i++)
        {
            cv::Mat roi1 = splitImg[i];
            CrossCenter cc(roi1, 100);
            int flag = HydrusParameters::crosshairAlgortihm;
            cv::Point2f c1 = cc.get_crossCenter(flag); //
            if (c1.x != 0 && c1.y != 0)
            {
                cenVec.push_back(c1);
            }
            if (cenVec.size() % 3 == 0)
            {
                for (int i = cenVec.size() - 3; i < cenVec.size(); i++)
                {
                    float x = cenVec[i].x + (cenVec.size() / 3 - 1) * (col / 3) + m_roiStart + rect.x;
                    float y = cenVec[i].y + (i % 3) * (row / 3) + m_roiStart + rect.y;
                    NewcenVec.emplace_back(x, y);
                }
            }
        }
        for (int i = 0; i < NewcenVec.size(); i++)
        {
            circle(img_draw, NewcenVec[i], 5, Scalar(255, 0, 255), -1, 8, 0);
        }
        m_img_draw = img_draw.clone();
    }
    return NewcenVec;
}

cv::Mat algorithm::MLCrosshairPara::CalculateCrosshairAngDis(vector<cv::Point2f> cenVecR, vector<cv::Point2f> cenVecG,
                                                             double &avgRG, double &maxRG)
{
    vector<double> disRG;
    cv::Mat RGmat = cv::Mat::zeros(Size(3, 3), CV_32FC1);
    double angRG = 0;
    if (cenVecR.size() == cenVecG.size())
    {
        for (int i = 0; i < cenVecR.size(); i++)
        {
            double dRGx = cenVecR[i].x - cenVecG[i].x;
            double dRGy = cenVecR[i].y - cenVecG[i].y;
            double dRG = sqrt(pow(dRGx, 2) + pow(dRGy, 2));
            double m_FocalLength = HydrusParameters::FocalLength;
            double m_pixel_size = m_para.pixel_size;
            angRG = atan(m_pixel_size * dRG / m_FocalLength) * 180.0 / CV_PI;
            disRG.push_back(angRG);
        }
        if (disRG.size() == 9)
        {
            int n = 0;
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    RGmat.at<float>(j, i) = disRG[n];
                    n++;
                }
            }
        }
        double minVal = 0;
        cv::minMaxIdx(RGmat, &minVal, &maxRG);
        if (disRG.size() > 0)
        {
            avgRG = accumulate(disRG.begin(), disRG.end(), 0.0) / disRG.size();
        }
    }
    return RGmat;
}
