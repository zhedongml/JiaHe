/*
 * For the intersection of polygons, the sides of the polygon must be given in a counterclockwise direction
 * It is also necessary to judge whether it is a convex hull or a concave hull, and call the corresponding function
 * Area union, just subtract the intersection from the area
 */

#include <opencv2/opencv.hpp>

#include "polygonIntersection.h"

 // Point2f p1[maxn], p2[maxn];
 // const int maxn = 300;
const float eps = 1e-6;

int algorithm::MLpolygonIntersection::dcmp(float x)
{
	if (x > eps)
		return 1;
	return x < -eps ? -1 : 0;
}
float algorithm::MLpolygonIntersection::cross(cv::Point2f a, cv::Point2f b, cv::Point2f c)
{
	return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
}
cv::Point2f algorithm::MLpolygonIntersection::intersection(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d)
{
	cv::Point2f p = a;
	float t = ((a.x - c.x) * (c.y - d.y) - (a.y - c.y) * (c.x - d.x)) /
		((a.x - b.x) * (c.y - d.y) - (a.y - b.y) * (c.x - d.x));
	p.x += (b.x - a.x) * t;
	p.y += (b.y - a.y) * t;
	return p;
}
// Calculate polygon area
float algorithm::MLpolygonIntersection::PolygonArea(cv::Point2f p[], int n)
{
	if (n < 3)
		return 0.0;
	float s = p[0].y * (p[n - 1].x - p[1].x);
	p[n] = p[0];
	for (int i = 1; i < n; ++i)
		s += p[i].y * (p[i - 1].x - p[i + 1].x);
	return fabs(s * 0.5);
}
float algorithm::MLpolygonIntersection::CPIA(cv::Point2f a[], cv::Point2f b[], int na,
	int nb) // ConvexPolygonIntersectArea
{
	cv::Point2f p[20], tmp[20];
	int tn, sflag, eflag;
	a[na] = a[0], b[nb] = b[0];
	memcpy(p, b, sizeof(cv::Point2f) * (nb + 1));
	for (int i = 0; i < na && nb > 2; i++)
	{
		sflag = dcmp(cross(a[i + 1], p[0], a[i]));
		for (int j = tn = 0; j < nb; j++, sflag = eflag)
		{
			if (sflag >= 0)
				tmp[tn++] = p[j];
			eflag = dcmp(cross(a[i + 1], p[j + 1], a[i]));
			if ((sflag ^ eflag) == -2)
				tmp[tn++] = intersection(a[i], a[i + 1], p[j], p[j + 1]); ///�󽻵�
		}
		memcpy(p, tmp, sizeof(cv::Point) * tn);
		nb = tn, p[nb] = p[0];
	}
	if (nb < 3)
		return 0.0;
	return PolygonArea(p, nb);
}
float algorithm::MLpolygonIntersection::SPIA(cv::Point2f a[], cv::Point2f b[], int na,
	int nb) /// SimplePolygonIntersectArea ���ô˺���
{
	int i, j;
	cv::Point2f t1[4], t2[4];
	float res = 0, num1, num2;
	a[na] = t1[0] = a[0], b[nb] = t2[0] = b[0];
	for (i = 2; i < na; i++)
	{
		t1[1] = a[i - 1], t1[2] = a[i];
		num1 = dcmp(cross(t1[1], t1[2], t1[0]));
		if (num1 < 0)
			swap(t1[1], t1[2]);
		for (j = 2; j < nb; j++)
		{
			t2[1] = b[j - 1], t2[2] = b[j];
			num2 = dcmp(cross(t2[1], t2[2], t2[0]));
			if (num2 < 0)
				swap(t2[1], t2[2]);
			res += CPIA(t1, t2, 3, 3) * num1 * num2;
		}
	}
	return res;
}
