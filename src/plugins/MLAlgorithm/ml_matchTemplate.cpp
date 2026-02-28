#include "ml_matchTemplate.h"
#include "line2Dup.h"
#include "line2Dup.h"
#include "opencv2/opencv.hpp"
#include "opencv2/world.hpp"
#include <assert.h>
#include <chrono>
#include <iostream>
#include <memory>
#include <opencv2\imgproc\types_c.h>
using namespace line2Dup;
using namespace std;
using namespace cv;
using namespace algorithm;
MatchTemplate::MatchTemplate()
{
}

MatchTemplate::~MatchTemplate()
{
}

ALGResult MatchTemplate::getMatchTemplateResult(cv::Mat src, cv::Mat templ, double score, vector<cv::Point2f> &loc1)
{
    ALGResult re;
    vector<cv::Point2f> loc;
    cv::Mat img_draw;
    if (src.channels() == 3)
    {
        cvtColor(src, m_src, COLOR_BGR2GRAY);
        img_draw = src.clone();
    }
    else
    {
        m_src = src.clone();
        cvtColor(src, img_draw, COLOR_GRAY2BGR);
    }
    m_img_draw = img_draw;
    m_template = templ;
    m_score = score;

    matchImageProcess(m_src);
    matchImageProcess(m_template);
    // TODO:
     prefix = "./config/templateImg/";
   // prefix = "../../app/config/templateImg/";
    if (!m_src.data)
    {
        re.success = false;
        re.errorMsg = "image is NULL";
        return re;
    }
    if (!m_template.data)
    {
        re.success = false;
        re.errorMsg = "template is NULL, load template image fail";
        return re;
    }

    string dir = prefix + "test_templ.yaml";
    if (!Is_File_Exist(dir))
    {
        angle_test("train", true);
    }
    try
    {
        loc = angle_test("test", true);
    }
    catch (...)
    {
        re.success = false;
        re.errorMsg = "match template  fail";
    }
    for (int i = 0; i < loc.size(); i++)
    {
        // Point2f(62,61)
        loc1.push_back(loc[0] + Point2f(117, 106));
        circle(img_draw, loc[i] + Point2f(117, 106), 5, Scalar(0, 0, 255), 1);
        // break;
    }
   // m_img_draw = img_draw.clone();
    if (loc.size() < 1)
    {
        re.success = false;
        re.errorMsg = "Fiducial detection fail";
    }
    return re;
}

cv::Mat MatchTemplate::getImgDraw()
{
    return m_img_draw;
}
class Timer
{
  public:
    Timer() : beg_(clock_::now())
    {
    }
    void reset()
    {
        beg_ = clock_::now();
    }
    double elapsed() const
    {
        return std::chrono::duration_cast<second_>(clock_::now() - beg_).count();
    }
    void out(std::string message = "")
    {
        double t = elapsed();
        std::cout << message << "\nelasped time:" << t << "s" << std::endl;
        reset();
    }

  private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1>> second_;
    std::chrono::time_point<clock_> beg_;
};
void algorithm::MatchTemplate::matchImageProcess(cv::Mat &img)
{
    cv::Mat gray;
    cv::GaussianBlur(img, gray, Size(3, 3), 0, 0);
    // cv::equalizeHist(gray, gray);
    cv::Mat element = cv::getStructuringElement(MORPH_RECT, Size(5, 5));
    cv::morphologyEx(gray, gray, MORPH_GRADIENT, element);
    cv::Mat imgth;
     cv::threshold(gray, imgth, 0, 255, THRESH_TRIANGLE);
    // NaiveRemoveNoise(imgth, 1000);
    // ContoursRemoveNoise(imgth, 1000);
     img = imgth.clone();
}
vector<cv::Point2f> MatchTemplate::angle_test(string mode, bool use_rot)
{
    vector<cv::Point2f> loc;
    line2Dup::Detector detector(128, {4, 8});
    if (mode == "train")
    {
        Mat img = m_template;
        Mat mask = Mat(img.size(), CV_8UC1, {255});
        // padding to avoid rotating out
        int padding = 100;
        cv::Mat padded_img = cv::Mat(img.rows + 2 * padding, img.cols + 2 * padding, img.type(), cv::Scalar::all(0));
        img.copyTo(padded_img(Rect(padding, padding, img.cols, img.rows)));

        cv::Mat padded_mask =
            cv::Mat(mask.rows + 2 * padding, mask.cols + 2 * padding, mask.type(), cv::Scalar::all(0));
        mask.copyTo(padded_mask(Rect(padding, padding, img.cols, img.rows)));

        shape_based_matching::shapeInfo_producer shapes(padded_img, padded_mask);
        shapes.angle_range = {0, 360};
        shapes.angle_step = m_angleStep;
        shapes.scale_range = {1}; // support just one
        shapes.produce_infos();
        std::vector<shape_based_matching::shapeInfo_producer::Info> infos_have_templ;
        string class_id = "test";

        bool is_first = true;

        // for other scales you want to re-extract points:
        // set shapes.scale_range then produce_infos; set is_first = false;
        int first_id = 0;
        float first_angle = 0;
        for (auto &info : shapes.infos)
        {
            Mat to_show = shapes.src_of(info);

            // std::cout << "\ninfo.angle: " << info.angle << std::endl;
            int templ_id;

            if (is_first)
            {
                templ_id = detector.addTemplate(shapes.src_of(info), class_id, shapes.mask_of(info));
                first_id = templ_id;
                first_angle = info.angle;

                if (use_rot)
                    is_first = false;
            }
            else
            {
                templ_id = detector.addTemplate_rotate(class_id, first_id, info.angle - first_angle,
                                                       {shapes.src.cols / 2.0f, shapes.src.rows / 2.0f});
            }

            if (templ_id != -1)
            {
                auto templ = detector.getTemplates("test", templ_id);
                for (int i = 0; i < templ[0].features.size(); i++)
                {
                    auto feat = templ[0].features[i];
                    cv::circle(to_show, {feat.x + templ[0].tl_x, feat.y + templ[0].tl_y}, 3, {0, 0, 255}, -1);
                }
                infos_have_templ.push_back(info);
            }
        }
        detector.writeClasses(prefix + "%s_templ.yaml");
        shapes.save_infos(infos_have_templ, prefix + "test_info.yaml");
        //  std::cout << "train end" << std::endl << std::endl;
    }
    else if (mode == "test")
    {
        std::vector<std::string> ids;
        ids.push_back("test");
        detector.readClasses(ids, prefix + "%s_templ.yaml");
        // angle & scale are saved here, fetched by match id
        auto infos = shape_based_matching::shapeInfo_producer::load_infos(prefix + "test_info.yaml");
        Mat test_img = m_src;
        int padding = 100;
        cv::Mat padded_img =
            cv::Mat(test_img.rows + 2 * padding, test_img.cols + 2 * padding, test_img.type(), cv::Scalar::all(0));
        test_img.copyTo(padded_img(Rect(padding, padding, test_img.cols, test_img.rows)));

        int stride = 16;
        int n = padded_img.rows / stride;
        int m = padded_img.cols / stride;
        Rect roi(0, 0, stride * m, stride * n);
        Mat img = padded_img(roi).clone();
        Timer timer;
        auto matches = detector.match(img, m_score, ids);
        timer.out();

        if (img.channels() == 1)
            cvtColor(img, img, CV_GRAY2BGR);
        cv::Mat img_draw = img.clone();

        size_t top5 = 5;
        if (top5 > matches.size())
            top5 = matches.size();
        //对结果进行筛选
        std::vector<Match> DesMatch;
        bool hasFlag = false;
        for (size_t i = 0; i < top5; i++)
        {
            hasFlag = false;
            if (i == 0)
            {
                DesMatch.push_back(matches[0]);
            }
            for (size_t j = 0; j < DesMatch.size(); j++)
            {
                if (abs(DesMatch[j].x - matches[i].x) < 5 && abs(DesMatch[j].y - matches[i].y) < 5)
                {
                    hasFlag = true;
                    if (matches[i].similarity > DesMatch[j].similarity)
                    { //当未保存的结果更优时，进行替换
                        DesMatch[j] = matches[i];
                        break;
                    }
                }
            }
            if (!hasFlag)
            {
                DesMatch.push_back(matches[i]);
            }
        }

        for (size_t i = 0; i < DesMatch.size(); i++)
        {
            auto match = DesMatch[i];
            auto templ = detector.getTemplates("test", match.template_id);

            // tl_x/y: template croping topleft corner when training

            float r_scaled = 200 / 2.0f * infos[match.template_id].scale;
            // scaling won't affect this, because it has been determined by warpAffine
            // cv::warpAffine(src, dst, rot_mat, src.size()); last param
            float train_img_half_width = 200 / 2.0f + 100;
            float train_img_half_height = 200 / 2.0f + 100;

            // center x,y of train_img in test img
            float x = match.x - templ[0].tl_x + train_img_half_width;
            float y = match.y - templ[0].tl_y + train_img_half_height;
            cv::Vec3b randColor;
            randColor[0] = rand() % 155 + 100;
            randColor[1] = rand() % 155 + 100;
            randColor[2] = rand() % 155 + 100;
            for (int i = 0; i < templ[0].features.size(); i++)
            {
                auto feat = templ[0].features[i];
                cv::circle(img, {feat.x + match.x, feat.y + match.y}, 3, randColor, -1);
            }

            cv::circle(img, Point(x, y), 3, Scalar(0, 0, 255), -1);
            loc.push_back(Point2f(x - 2 * padding, y - 2 * padding));
            cv::circle(img, Point(x - padding, y - padding), 3, Scalar(0, 255, 0), -1);
            cv::putText(img, to_string(int(round(match.similarity))), Point(match.x + r_scaled - 10, match.y - 3),
                        FONT_HERSHEY_PLAIN, 2, randColor);

            cv::RotatedRect rotatedRectangle({x, y}, {2 * r_scaled, 2 * r_scaled}, -infos[match.template_id].angle);
            // cv::RotatedRect rotatedRectangle({ x, y }, { 200, 200}, -infos[match.template_id].angle);

            cv::Point2f vertices[4];
            rotatedRectangle.points(vertices);
            for (int i = 0; i < 4; i++)
            {
                int next = (i + 1 == 4) ? 0 : (i + 1);
                cv::line(img, vertices[i], vertices[next], randColor, 2);
            }
        }
    }

    return loc;
}
