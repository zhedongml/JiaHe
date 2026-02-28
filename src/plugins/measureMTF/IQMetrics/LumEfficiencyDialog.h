#pragma once

#include <QDialog>
#include "ui_LumEfficiencyDialog.h"

class LumEfficiencyDialog : public QDialog
{
	Q_OBJECT

public:
	LumEfficiencyDialog(QWidget* parent = nullptr);
	~LumEfficiencyDialog();

	void setResult(cv::Mat image, double lumEfficiency);

private:
	void initUI();

public:
	Ui::LumEfficiencyDialogClass ui;
};
