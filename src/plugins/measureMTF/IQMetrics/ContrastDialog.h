#pragma once

#include <QDialog>
#include "ui_ContrastDialog.h"

class ContrastDialog : public QDialog
{
	Q_OBJECT

public:
	ContrastDialog(QWidget* parent = nullptr);
	~ContrastDialog();

	void setResult(cv::Mat imagePos, cv::Mat imageNeg, double contrast);

private:
	void initUI();

public:
	Ui::ContrastDialogClass ui;
};
