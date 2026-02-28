#pragma once

#include <QDialog>
#include "ui_ChromacityDialog.h"
//#include "MLAlgorithm/ml_luminance.h"

//using namespace algorithm;

class ChromacityDialog : public QDialog
{
	Q_OBJECT

public:
	ChromacityDialog(QWidget* parent = nullptr);
	~ChromacityDialog();

	//void setResult(cv::Mat image, ChromacityRE result, cv::Mat imageY);

private:
	void initUI();

public:
	Ui::ChromacityDialogClass ui;
};
