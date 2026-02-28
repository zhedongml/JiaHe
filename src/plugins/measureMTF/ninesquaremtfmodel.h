#pragma once

#include <QObject>
#include <QVector>

#include "crossimgmtfmodel.h"
#include "opencv2/opencv.hpp"
#include "Result.h"

class NineSquareMTFModel: public QObject
{
  Q_OBJECT

public:
  static NineSquareMTFModel* getInstance(QObject* parent = nullptr);
  void setNineSquareImg(cv::Mat img);
  void calculateMTF(bool isAutoMode, QList<int> list = QList<int>());
  void calculateMTFByAutoIdentifyNineCross(bool isAutoMode, QList<int> list = QList<int>());
  void generateResult();
  bool createResutCSVFile(QString cvFileName);
  bool getIsOffLineImg();
  void setIsOffLineImg(bool val);
  void setOfflineImgPath(QString imgPath);
  void setDUTBarCode(QString val);
  Result thruFocus(float expectFocus = -1);
  QString matchLogAll = "";
  void setLedColor(QString color);
  void setEyeboxNum(QString num);
  void recipeStop(bool isStop);

  std::vector<cv::Rect> autoIdentifyNineCrossRect(cv::Mat image);
  std::vector<cv::Mat> autoIdentifyNineCross(cv::Mat image,cv::Mat&imgdraw);

private:
  void clearResult();
  cv::Mat nineSquareImg;
  QVector<PassResult*> resultVec;
  static NineSquareMTFModel* self;
  NineSquareMTFModel(QObject* parent = nullptr);
  ~NineSquareMTFModel();
  bool useOffLineImage = false;
  QString offlineImagPath = "";
  QString dutId = "";
  bool saveMTFData = true;
  void writeMatchLogAll(QString writePath, QString writePathAll);
  float vid = 0;
  QString ledColor = "w";
  QString eyeboxNum = "5";
};
