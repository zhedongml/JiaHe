#pragma once
#include <opencv2/core/base.hpp>
#include <opencv2/core/mat.hpp>
#ifndef MLALGORITHM_MLPOLYGONINTERSETION_H_
#define MLALGORITHM_MLPOLYGONINTERSETION_H_
namespace algorithm
{
class MLpolygonIntersection
{
  public:
    int dcmp(float x);
    float cross(cv::Point2f a, cv::Point2f b, cv::Point2f c);
    cv::Point2f intersection(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d);
    float PolygonArea(cv::Point2f p[], int n);
    float CPIA(cv::Point2f a[], cv::Point2f b[], int na, int nb);
    float SPIA(cv::Point2f a[], cv::Point2f b[], int na, int nb);
};
} // namespace algorithm
#endif // !MLALGORITHM_MLIMAGEPUBLIC_H_
