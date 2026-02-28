#include "ml_exposure_auto.h"
#include <string>

MLExposureAuto::MLExposureAuto(float dynamic_range, double dark_level, int dark_time, int min_time, int max_time, MLPixelFormat format)
{
    this->dynamic_range_ = dynamic_range;
    this->black_level_ = dark_level;
    this->black_time_ = dark_time;
    this->min_time_ = min_time;
    this->max_time_ = max_time;
    this->format_ = format;
    max_value_ = MaxCamValue(this->format_);
    instance_ = NULL;
}

MLExposureAuto::~MLExposureAuto()
{
}

bool Descend(int a, int b)
{
    return a > b;

}



//Mean value of max first N pixels 
double MLExposureAuto::MaxNValue(Mat img, int n)
{
    if (img.empty())
    {
        return 0;
    }
    double maxV = 0;
    int sum = 0;
    Mat img_c;
    img.convertTo(img_c, CV_32SC1);
    int w = img_c.cols;
    int h = img_c.rows;
    int* p = img_c.ptr<int>(0);
    std::sort(p, p + n);
    std::reverse(p, p + n);

    for (int i = n; i < w * h; i++)
    {
        int j = 0;
        for (j = 0; j < n; j++)
        {
            if (p[i] > p[j])
            {
                p[j] = p[i];
                break;
            }
        }

    }

    for (int i = 0; i < n; i++)
    {
        sum += p[i];
    }
    maxV = double(sum) / double(n);

    return maxV;

}


// Max value of pixels based on the format of image
int MLExposureAuto::MaxCamValue(MLPixelFormat format)
{
    int max;

    switch (format)
    {
    case MLPixelFormat::kMLMono8:
    {
        max = pow(2, 8) - 1;
        break;
    }
    case MLPixelFormat::kMLMono12:
    {
        max = pow(2, 12) - 1;
    }
    case MLPixelFormat::kMLMono16:
    {
        max = pow(2, 16) - 1;
        break;
    }
    }
    return max;

}

// Number of pixels higher than threhold_value
int MLExposureAuto::NumOver(Mat img, int threhold_value)
{
    Mat img_th;
    int num;
    //threshold(img, img_th, threhold_value, 1, CV_THRESH_BINARY);
    img_th = img > threhold_value;
    num = countNonZero(img_th);
    return num;
}

double MLExposureAuto::CalculateTime(int vtime, double value)
{
    double cal_time = dynamic_range_ * max_value_ / ((value - vtime / black_time_ * black_level_) / vtime + black_level_ / black_time_);
    //int time = floor(cal_time) * (50 / 3);
    return cal_time;
}


int MLExposureAuto::SingleAutoExposure(int initial_time,double max_value, Mat img, CaptureCallback capture, int num)
{
    int time = initial_time;
    double maxNV = max_value;
    std::vector<double> k;
    std::vector<double> k_temp;
    double ratio;
    Mat roi = img;
    int num_high;
    int num_mid;
    int iter_num = 0;

    /*Iteration to find exposure time*/
    int temp_time = initial_time;
    int iter_over = 0;
    std::vector<int> times;
    /*k_temp.push_back(1.0);*/
    while (maxNV > 0.95 * max_value_ || maxNV < 0.2 * max_value_)
    {
        if (iter_num > 10)	//The maximum number of iterations
            break;



        if (maxNV > 0.95 * max_value_)	//Greater than threshold
        {
            num_high = NumOver(roi, 0.95 * max_value_);
            num_mid = NumOver(roi, 0.5 * max_value_);
            ratio = 1 - double(num_high) / double(num_mid);
            k_temp.push_back(ratio);
            if (iter_over > 0)
            {
                if (k_temp[iter_over] / k_temp[iter_over - 1] <= 1)
                {
                    if (k_temp[iter_over] / k_temp[iter_over - 1] > 0.9)
                        ratio /= 10;
                    else if (k_temp[iter_over] / k_temp[iter_over - 1] > 0.8)
                        ratio /= 5;
                    else if (k_temp[iter_over] / k_temp[iter_over - 1] > 0.7)
                        ratio /= 2;
                }
                else
                    ratio /= 5;
            }
            iter_over++;

        }
        else if (maxNV > black_level_)
        {
            ratio = (0.5 * max_value_ - temp_time / black_time_ * black_level_) / (maxNV - temp_time / black_time_ * black_level_);
        }
        else
        {
            ratio = 100;
        }

        k.push_back(ratio);
        temp_time *= k[iter_num];
        temp_time = floor(temp_time);

        temp_time = (temp_time < min_time_) ? min_time_ : temp_time;
        temp_time = (temp_time > max_time_) ? max_time_ : temp_time;
        times.push_back(temp_time);
        if (iter_num > 0 && ((times[iter_num - 1] == min_time_ && times[iter_num] == min_time_) || (times[iter_num - 1] == max_time_ && times[iter_num] == max_time_)))
            break;

        roi = capture(instance_, temp_time, false);
        maxNV = MaxNValue(roi, num);
        iter_num++;
    }

    time = CalculateTime(temp_time, maxNV);
    return time;
}


int MLExposureAuto::MLAutoExposure(void* instance, CaptureCallback capture, int max_num)
{
    int time = 300000;
    int satisfied_num = 0;
    double max_n_value;
    this->instance_ = instance;
    Mat rois = capture(instance, time, true);	//Using AutoExposure from camera to capture rois.

    max_n_value = MaxNValue(rois, max_num);
    if (max_n_value < 0.95 * max_value_ && max_n_value>0.2 * max_value_)
    {
        int my_time = CalculateTime(time, max_n_value);

        my_time = (my_time < min_time_) ? min_time_ : my_time;
        my_time = (my_time > max_time_) ? max_time_ : my_time;

        return my_time;
    }

    int my_time = SingleAutoExposure(time, max_n_value, rois, capture, max_num);

    my_time = (my_time < min_time_) ? min_time_ : my_time;
    my_time = (my_time > max_time_) ? max_time_ : my_time;

    return my_time;
}


Mat GetFromFile(int& exposure_time, bool is_auto)
{
    std::string file = ".\\16_L\\" + std::to_string(exposure_time) + ".tiff";
    Mat img = imread(file, -1);

    return img;
}


/*int main()
{

Mat dark = imread("dark_500000_16.tiff", -1);
if (dark.channels() == 3)
cvtColor(dark, dark, CV_BGR2GRAY);

Mat gray, mat_mean, mat_stddev;
meanStdDev(dark, mat_mean, mat_stddev);
double m, s;
m = mat_mean.at<double>(0, 0);
Scalar mm = mean(dark);
m = mm.val[0];

AutoExpsure ML_Exposure(m, 500000, 24, 5000000, ML_MONO16);
int t1 = clock();
int et = ML_Exposure.MLAutoExposure(GetFromFile);
int t2 = clock();
int t = t2 - t1;
Mat img = GetFromFile(et, false);

int maxn = ML_Exposure.MaxNValue(img, 5);

return 0;
}*/