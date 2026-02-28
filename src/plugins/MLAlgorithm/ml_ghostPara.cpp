#include "ml_ghostPara.h"

algorithm::MLGhostPara::MLGhostPara()
{
}

algorithm::MLGhostPara::~MLGhostPara()
{
}

void algorithm::MLGhostPara::SetGhostImage(cv::Mat img_ghost)
{

    cv::Rect rectangle = hydrusConfig.GetNewPara(img_ghost).rect;
    if (rectangle.x != 0 || rectangle.y != 0)
    {
        m_img_ghost = img_ghost(rectangle);
    }
    else
    {
        m_img_ghost = img_ghost;
    }
    m_para = hydrusConfig.m_para;
}

std::vector<double> algorithm::MLGhostPara::GetStrengthRatio()
{
    std::vector<double> result;
    result = StrengthRatio(m_img_ghost);
    return result;
}

cv::Mat algorithm::MLGhostPara::getImgDraw()
{
    return m_img_draw;
}

std::vector<double> algorithm::MLGhostPara::StrengthRatio(cv::Mat img)
{
    vector<double> re;
    if (img.data != NULL)
    {
    }
    return re;
}
