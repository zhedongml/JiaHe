#pragma once

#include <QDialog>
#include "ui_LuminanceDialog.h"
//#include "MLAlgorithm/ml_chromaPara.h"
#include"ML_Luminance.h"
class LuminanceDialog : public QDialog
{
	Q_OBJECT

public:
	LuminanceDialog(QWidget* parent = nullptr);
	~LuminanceDialog();

	void setResult(cv::Mat image, MLIQMetrics::LuminanceRe lu);

private:
	void initUI();

private:
	Ui::LuminanceDialogClass ui;
};
