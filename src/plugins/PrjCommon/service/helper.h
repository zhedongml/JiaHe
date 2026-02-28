#ifndef COMMON_HELPER_H_
#define COMMON_HELPER_H_
#include <queue>
#include <vector>
using std::priority_queue;
using std::greater;
using std::vector;

#include <opencv2/opencv.hpp>

using cv::Mat;

namespace Helper
{
    inline double GetGrayLevel(Mat img, int n)
    {
        if (img.empty()) return 0;

        if (img.type() != CV_8U) return 0;

        int sum = 0;
        int w = img.cols;
        int h = img.rows;
        unsigned char* p = img.data;

        priority_queue <unsigned char, vector<unsigned char>, greater<unsigned char> > q;
        for (int i = 0; i < n; i++)
        {
            q.push(*p);
            ++p;
        }

        for (int i = n; i < w * h; i++)
        {
            if ((*p) > q.top())
            {
                q.pop();
                q.push(*p);
            }
            ++p;
        }

        while (!q.empty())
        {
            sum += q.top();
            q.pop();
        }
        return double(sum) / n;
    }

    inline double GetGrayValue(cv::Mat image, int pixelNumbers)
    {
        double maxV = 0;
        int sum = 0;
        cv::Mat img_c;
        image.convertTo(img_c, CV_32SC1);
        int w = img_c.cols;
        int h = img_c.rows;
        int* p = img_c.ptr<int>(0);
        std::sort(p, p + pixelNumbers);
        std::reverse(p, p + pixelNumbers);

        for (int i = pixelNumbers; i < w * h; i++)
        {
            int j = 0;
            for (j = 0; j < pixelNumbers; j++)
            {
                if (p[i] > p[j])
                {
                    p[j] = p[i];
                    break;
                }
            }

        }

        for (int i = 0; i < pixelNumbers; i++)
        {
            sum += p[i];
        }
        maxV = double(sum) / double(pixelNumbers);

        return maxV;
    }

    inline double GetGrayValue3(cv::Mat img, int pixelNumbers)
    {
        if (img.empty())
        {
            return 0;
        }
        int max_value = 255;
        if (img.type() == CV_8U)
        {
            max_value = 255;
        }
        else if (img.type() == CV_16U)
        {
            max_value = 4095;
        }

        {
            // update code 2024-2-1 
            cv::Mat img1; 
            img1 = img.reshape(1, img.rows * img.cols);
            cv::Mat imgSort;
            cv::sort(img1, imgSort, cv::SORT_EVERY_COLUMN + cv::SORT_DESCENDING);
            cv::Mat a1 = imgSort(cv::Rect(0, 0, 1, pixelNumbers)).clone();
            cv::Scalar m1 = cv::mean(a1);
            double gray = m1(0);
            return gray;
        }


        //¶¨ÒåÇóÖ±·½Í¼µÄÍ¨µÀÊýÄ¿£¬´Ó0¿ªÊ¼Ë÷Òý
        int channels[] = { 0 };
        const int histSize[] = { max_value + 1 };
        //Ã¿Ò»Î¬binµÄ±ä»¯·¶Î§
        float range[] = { 0, max_value + 1 };
        const float* ranges[] = { range };

        cv::Mat hist;
        cv::calcHist(&img, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);//cvÖÐÊÇcvCalcHist

        int sum = 0;
        int tmpSum = 0;
        std::map<int, int> GraySum;
        int grayNum = 0;

        for (int gray = max_value; gray > -1; gray--)
        {
            float bin_val = hist.at<float>(gray);
            if (bin_val != 0)
            {
                sum = bin_val + sum;
                GraySum[gray] = bin_val;
                if (sum >= pixelNumbers)
                {
                    grayNum = grayNum + (pixelNumbers - tmpSum) * gray;
                    break;
                }
                grayNum = bin_val * gray + grayNum;
                tmpSum = sum;
            }
        }

        double aveGray = (double)grayNum / (double)pixelNumbers;
        return aveGray;
       
        /*if (GraySum.size() == 1)
        {
            return GraySum.begin()->first;
        }
        else
        {
            double sum1 = 0;
            int pixelNum = 0;
            std::map<int, int>::iterator it = GraySum.end();
            it--;
            for (int i = 0; i < GraySum.size() - 1; i++)
            {
                sum1 = sum1 + (it->first * it->second);
                pixelNum = pixelNum + it->second;
                it--;
            }
            sum1 = sum1 + (it->first * (pixelNumbers - pixelNum));
            double maxV = double(sum1) / double(pixelNumbers);

            return maxV;
        }*/

        
    }

    inline double GetGrayValue2(cv::Mat img, int pixelNumbers)
    {
        if (img.empty())
        {
            return 0;
        }

        int max_value = 255;
        if (img.type() == CV_8U)
        {
            max_value = 255;
        }
        else if (img.type() == CV_16U)
        {
            max_value = 4095;
        }
        int channels[] = { 0 };
        const int histSize[] = { max_value + 1 };

        float range[] = { 0, max_value + 1 };
        const float* ranges[] = { range };

        cv::Mat hist;
        cv::calcHist(&img, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

        int sum = 0;
        int tmpSum = 0;
        std::map<int, int> GraySum;
        int grayNum = 0;

        for (int gray = max_value; gray > -1; gray--)
        {
            float bin_val = hist.at<float>(gray);
            if (bin_val != 0)
            {
                sum = bin_val + sum;
                GraySum[gray] = bin_val;
                if (sum >= pixelNumbers)
                {
                    grayNum = grayNum + (pixelNumbers - tmpSum) * gray;
                    break;
                }
                grayNum = bin_val * gray + grayNum;
                tmpSum = sum;
            }
        }
        double aveGray = (double)grayNum / (double)pixelNumbers;
        return aveGray;
    }

    inline double GreaterNValue(cv::Mat img, int n)
    {
        if (img.empty())
        {
            return 0;
        }
        int max_value = 4095;
        if (img.type() == CV_8U)
        {
            max_value = 255;
        }
        else if (img.type() == CV_16U)
        {
            max_value = 4095;
        }
        //¶¨ÒåÇóÖ±·½Í¼µÄÍ¨µÀÊýÄ¿£¬´Ó0¿ªÊ¼Ë÷Òý
        int channels[] = { 0 };
        const int histSize[] = { max_value + 1 };
        //Ã¿Ò»Î¬binµÄ±ä»¯·¶Î§
        float range[] = { 0, max_value + 1 };
        const float* ranges[] = { range };

        cv::Mat hist;
        cv::calcHist(&img, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);//cvÖÐÊÇcvCalcHist

        for (int gray = max_value; gray > -1; gray--)
        {
            float bin_val = hist.at<float>(gray);
            if (bin_val > n)
            {
                return gray;
            }
        }

        return 0;
    }
}

#endif //COMMON_HELPER_H_