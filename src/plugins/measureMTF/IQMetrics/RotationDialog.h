#pragma once

#include <QDialog>
#include "ui_RotationDialog.h"
//#include "MLAlgorithm/ml_crosshairpara.h"

class RotationDialog : public QDialog
{
	Q_OBJECT

public:
	RotationDialog(QWidget* parent = nullptr);
	~RotationDialog();

	void setResult(cv::Mat image, float degree);

private:
	void initUI();

private:
	Ui::RotationDialogClass ui;
};
