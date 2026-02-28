#pragma once

#include <QDialog>
#include "ui_CraDialog.h"
//#include "MLAlgorithm/ml_crosshairpara.h"
//#include"ml_CRA.h"
#include"ML_FOVOffset.h"
#include"ML_Rotation.h"
#include "Result.h"

using namespace MLIQMetrics;

class CraDialog : public QDialog
{
	Q_OBJECT

public:
	CraDialog(QWidget* parent = nullptr);
	~CraDialog();

	void setResult(cv::Mat image, FovOffsetRe craResult);

private:
	void initUI();

public:
	Ui::CraDialogClass ui;
};
