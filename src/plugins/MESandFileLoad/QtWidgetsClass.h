#pragma once

#include <QDialog>
#include <QMainWindow>
#include <QCloseEvent>
#include "MyHTTPClient.h"

#include <QDialog>
#include <QEventLoop>
#include "Result.h"
#include <QMap>


namespace Ui {
	class QtWidgetsClass;
	class QtWidgetsClassClass;
}

class MesDialog : public QObject
{
	Q_OBJECT
public:
	static MesDialog& instance();
	~MesDialog();

	Result showDialog();

private:
	MesDialog(QObject* parent = nullptr);

signals:
	void showThreadSignal(QEventLoop* loop);

private slots:
	void showThreadSlot(QEventLoop* loop);

private:
	Result m_result;
};


class QtWidgetsClass : public QDialog
{
	Q_OBJECT

public:
	explicit QtWidgetsClass(QWidget* parent = nullptr);
	~QtWidgetsClass();

	Result getControlData(ControlData& outData) const;

public:
	static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private slots:
	void on_addRow_clicked();
	void on_removeRow_clicked();
	void on_btn_ok_clicked();
	void on_btn_cancel_clicked();

signals:
	void dialogClosed(const ControlData& data);

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void init();

private:
	Ui::QtWidgetsClassClass* ui;
	MyHTTPClient* httpClient;
	bool isConnect = false;
};

