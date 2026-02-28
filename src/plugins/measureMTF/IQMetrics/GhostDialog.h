#pragma once

#include <QDialog>
#include "ui_GhostDialog.h"

class GhostDialog : public QDialog
{
	Q_OBJECT

public:
	GhostDialog(QWidget* parent = nullptr);
	~GhostDialog();

	void setResult(cv::Mat image, cv::Mat imageOverExpose);

private:
	void initUI();

public:
	Ui::GhostDialogClass ui;
};
