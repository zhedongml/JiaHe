#pragma once

#include <QDialog>
#include "ui_DutEyeboxWidget.h"
#include "opencv2/opencv.hpp"
#include "Result.h"

class DutEyeboxWidget : public QDialog
{
	Q_OBJECT

public:
	DutEyeboxWidget(QWidget *parent = nullptr);
	~DutEyeboxWidget();

	void updateEyeboxs(std::vector<cv::Point3f> eyeboxs);
    std::vector<cv::Point3f> getEyeboxs();

private slots:
	void on_comboBox_eyeboxNumber_activated(int index);
	void on_btn_ok_clicked();
    void on_btn_cancel_clicked();
    void on_btn_importCsv_clicked();
    void on_btn_exportCsv_clicked();

private:
	void init();
	void initTableStyle();
    void initTable();
	void showRowCount(int rowCount);
    Result judgetUiData();

private:
	Ui::DutEyeboxWidgetClass ui;
	const int DECIMAL = 3;
};
