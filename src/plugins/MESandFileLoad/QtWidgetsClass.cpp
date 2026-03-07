// -*- coding: utf-8 -*-
#include "QtWidgetsClass.h"
#include <QFile>
#include <QDateTime>
#include <QMessageBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QString>
#include "Result.h"

#include "ui_QtWidgetsClass.h"
#include "MesMode.h"

QtWidgetsClass::QtWidgetsClass(QWidget *parent)
	: QDialog(parent), httpClient(new MyHTTPClient(this)),
	ui(new Ui::QtWidgetsClassClass)
{
	ui->setupUi(this);
	setWindowTitle("MES Information");

	this->setWindowModality(Qt::WindowModal);

	qInstallMessageHandler(&QtWidgetsClass::messageHandler);
}

QtWidgetsClass::~QtWidgetsClass()
{}

Result QtWidgetsClass::getControlData(ControlData& outData) const
{
	try
	{
		outData.qualifiedMolds = ui->cbquali->isChecked();
		outData.giveWayMolds = ui->cbconcess->isChecked();
		outData.lineStatus = ui->leStatus->text();
		outData.lineMac = ui->leMac->text();

		// 处理表格数据
		for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
			if (auto keyItem = ui->tableWidget->item(row, 0)) {
				if (auto valueItem = ui->tableWidget->item(row, 1)) {
					outData.tableData.insert(keyItem->text(), valueItem->text());
				}
			}
		}
		return Result(true,"");
	}
	catch (const std::exception& e)
	{
		return Result(false, e.what());
	}
}

void QtWidgetsClass::closeEvent(QCloseEvent* event)
{
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, tr("close window"), tr("Close the window and save the current parameters?"), QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		ControlData data;
		Result ret = getControlData(data);
		if (ret.success) {
			emit dialogClosed(data);
		}
	}
	event->accept();
}

void QtWidgetsClass::init()
{
	
}

void QtWidgetsClass::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
	QFile file("debug.log");
	if (file.open(QIODevice::Append | QIODevice::Text)) {
		QTextStream out(&file);
		switch (type) {
		case QtDebugMsg:
			out << "Debug: " << msg << "\n";
			break;
		case QtWarningMsg:
			out << "Warning: " << msg << "\n";
			break;
		case QtCriticalMsg:
			out << "Critical: " << msg << "\n";
			break;
		case QtFatalMsg:
			out << "Fatal: " << msg << "\n";
			//abort(); // 可选，终止程序
		}
		file.close();
	}
}


void QtWidgetsClass::on_addRow_clicked()
{
	int rowCount = ui->tableWidget->rowCount();
	ui->tableWidget->insertRow(rowCount);
	for (int column = 0; column < ui->tableWidget->columnCount(); ++column) {
		QTableWidgetItem* item = new QTableWidgetItem();
		ui->tableWidget->setItem(rowCount, column, item);
	}
}

void QtWidgetsClass::on_removeRow_clicked()
{
	int currentRow = ui->tableWidget->currentRow();
	if (currentRow != -1) {
		ui->tableWidget->removeRow(currentRow);
	}
	else
	{
		QMessageBox::warning(this, "error", "Please select a row");
	}
}

void QtWidgetsClass::on_btn_ok_clicked()
{
	ControlData outData;
	Result ret = getControlData(outData);
	if(!ret.success){
		qCritical() << QString::fromStdString("Get MES base info error " + ret.errorMsg);
	}

	MesMode::instance().setMesBaseInfo(outData);
	accept();
}

void QtWidgetsClass::on_btn_cancel_clicked()
{
	reject();
}

MesDialog& MesDialog::instance()
{
	static MesDialog self;
	return self;
}

MesDialog::MesDialog(QObject* parent):
	QObject(parent)
{
	connect(this, SIGNAL(showThreadSignal(QEventLoop*)), this, SLOT(showThreadSlot(QEventLoop*)));
}

void MesDialog::showThreadSlot(QEventLoop* loop)
{
	QtWidgetsClass *dialog = new QtWidgetsClass();
	int ok = dialog->exec();
	if (ok == QDialog::Accepted) {
		m_result = Result();
	}
	else {
		m_result = Result(false, "Operation canceled.");
	}

	if (loop) {
		loop->quit();
	}

	delete dialog;
	dialog = nullptr;
}

MesDialog::~MesDialog()
{
}

Result MesDialog::showDialog()
{
	QEventLoop loop;
	emit showThreadSignal(&loop);
	loop.exec();
	return m_result;
}
