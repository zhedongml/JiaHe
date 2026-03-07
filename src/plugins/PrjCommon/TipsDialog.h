#pragma once

#include <QDialog>
#include "ui_TipsDialog.h"

class TipsDialog : public QDialog
{
	Q_OBJECT

public:
	TipsDialog(QString title, QString msg, 
			   QString yesInfo, QString noInfo, QWidget *parent = nullptr);
	~TipsDialog();

private slots:
	void on_pushButton_OK_clicked();
	void on_pushButton_Cancel_clicked();

private:
	Ui::TipsDialog ui;
	QString m_Msg;
};

