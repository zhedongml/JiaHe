#pragma once

#include <QMainWindow>
#include "ui_DecoderWidget.h"
#include "DecoderModel.h"
#include <Core/itoolbox.h>
#include <QGraphicsPixmapItem>

class DecoderWidget : public Core::IToolBox
{
	Q_OBJECT

public:
	DecoderWidget(QString toolboxName = "", QWidget *parent = nullptr);
	~DecoderWidget();

private slots:
	void on_btn_connect_clicked();
	void on_btn_disconnect_clicked();
	void on_btn_read_clicked();
	void on_btn_captureImage_clicked();

	void connectStatus(bool status, QString msg);

private:
	Ui::DecoderWidgetClass ui;
	DecoderModel *m_model;
	QGraphicsPixmapItem* m_item = nullptr;
};
