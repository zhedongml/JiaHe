#include "DutEyeboxWidget.h"
#include "CustomSpinboxDelegate.h"
#include <QMessagebox>
#include "FileCsv.h"
#include <QFileDialog>

DutEyeboxWidget::DutEyeboxWidget(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	init();
}

DutEyeboxWidget::~DutEyeboxWidget()
{
}

void DutEyeboxWidget::updateEyeboxs(std::vector<cv::Point3f> eyeboxs)
{
    int rowCount = ui.tableWidget_eyeboxs->rowCount();
    int colCount = ui.tableWidget_eyeboxs->columnCount();
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            ui.tableWidget_eyeboxs->item(row, col)->setText("0");
        }
    }

    ui.comboBox_eyeboxNumber->setCurrentText(QString::number(eyeboxs.size()));

    showRowCount(eyeboxs.size());
    for(int i = 0; i < eyeboxs.size(); ++i){
        ui.tableWidget_eyeboxs->item(i, 0)->setText(QString::number(eyeboxs[i].x));
        ui.tableWidget_eyeboxs->item(i, 1)->setText(QString::number(eyeboxs[i].y));
        ui.tableWidget_eyeboxs->item(i, 2)->setText(QString::number(eyeboxs[i].z));
    }
}

std::vector<cv::Point3f> DutEyeboxWidget::getEyeboxs()
{
    std::vector<cv::Point3f> list;
    int rowCount = ui.comboBox_eyeboxNumber->currentText().toInt();
    for (int i = 0; i < rowCount; ++i){
        cv::Point3f point;
        point.x = ui.tableWidget_eyeboxs->item(i, 0)->text().toFloat();
        point.y = ui.tableWidget_eyeboxs->item(i, 1)->text().toFloat();
        point.z = ui.tableWidget_eyeboxs->item(i, 2)->text().toFloat();
        list.push_back(point);
    }

    return list;
}

void DutEyeboxWidget::on_btn_ok_clicked()
{
    Result ret = judgetUiData();
    if (!ret.success){
        QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }

    accept();
}

void DutEyeboxWidget::on_btn_cancel_clicked()
{
    reject();
}

void DutEyeboxWidget::on_btn_importCsv_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("Select file"), ".", QStringLiteral("Files(*.csv)"));
    if (fileName.isEmpty())
    {
        return;
    }

    QList<cv::Point3f> eyeboxs;
    Result ret = FileCsv().readEyeboxs(fileName, eyeboxs);
    if(!ret.success){
        QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                             QMessageBox::NoButton);
        return;
    }

    ui.comboBox_eyeboxNumber->setCurrentIndex(eyeboxs.size() - 1);
    showRowCount(ui.comboBox_eyeboxNumber->currentText().toInt());
    for (int row = 0; row < eyeboxs.size(); ++row)
    {
        ui.tableWidget_eyeboxs->item(row, 0)->setText(QString::number(eyeboxs[row].x, 'f', 3));
        ui.tableWidget_eyeboxs->item(row, 1)->setText(QString::number(eyeboxs[row].y, 'f', 3));
        ui.tableWidget_eyeboxs->item(row, 2)->setText(QString::number(eyeboxs[row].z, 'f', 3));
    }
}

void DutEyeboxWidget::on_btn_exportCsv_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, QStringLiteral("Export CSV"), ".", QStringLiteral("Files(*.csv)"));
    if (fileName.isEmpty())
    {
        return;
    }

    std::vector<cv::Point3f> eyeboxs = getEyeboxs();
    QList<cv::Point3f> list;
    for (cv::Point3f eye : eyeboxs){
        list.append(eye);
    }

    Result ret = FileCsv().writeEyeboxs(fileName, list);
    if (!ret.success)
    {
        QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                             QMessageBox::NoButton);
        return;
    }
}

void DutEyeboxWidget::on_comboBox_eyeboxNumber_activated(int index)
{
    showRowCount(ui.comboBox_eyeboxNumber->currentText().toInt());
}

void DutEyeboxWidget::init()
{
	for(int i = 1; i < 16; ++i){
        ui.comboBox_eyeboxNumber->addItem(QString::number(i));
	}
    ui.comboBox_eyeboxNumber->setCurrentText("9");
    on_comboBox_eyeboxNumber_activated(8);

    initTableStyle();
    initTable();

    setWindowFlags(windowFlags() | Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);

    ui.btn_cancel->setFocus();
}

void DutEyeboxWidget::initTableStyle()
{
    QString qssTV = "QTableWidget::item:hover{background-color:rgb(92,188,227,200)}"
                    "QTableWidget::item:selected{background-color:#1B89A1}"
                    "QHeaderView::section,QTableCornerButton:section{ \
                     padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
                     border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                     background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                    "QTableWidget{background-color:white;border:none;}";

    ui.tableWidget_eyeboxs->horizontalHeader()->setFont(QFont("song", 12));
    QFont font = ui.tableWidget_eyeboxs->horizontalHeader()->font();
    font.setBold(true);
    ui.tableWidget_eyeboxs->horizontalHeader()->setFont(font);

    ui.tableWidget_eyeboxs->setStyleSheet(qssTV);

    setWindowTitle("DUT Eyebox Config");
}

void DutEyeboxWidget::initTable()
{
    QStringList HStrList;
    HStrList << "X"
             << "Y"
             << "Z";
    ui.tableWidget_eyeboxs->setColumnCount(HStrList.count());
    ui.tableWidget_eyeboxs->setHorizontalHeaderLabels(HStrList);

    QStringList VStrList;
    for (int i = 1; i <= ui.comboBox_eyeboxNumber->count(); ++i)
    {
        VStrList << QString::number(i);
    }
    ui.tableWidget_eyeboxs->setRowCount(VStrList.size());
    ui.tableWidget_eyeboxs->setVerticalHeaderLabels(VStrList);

    int rowCount = ui.tableWidget_eyeboxs->rowCount();
    int colCount = ui.tableWidget_eyeboxs->columnCount();
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            QTableWidgetItem *xItem = new QTableWidgetItem("0");
            ui.tableWidget_eyeboxs->setItem(row, col, xItem);
        }
    }

    ui.tableWidget_eyeboxs->setItemDelegate(new CustomSpinboxDelegate());
}

void DutEyeboxWidget::showRowCount(int rowCount)
{
    int rowAll = ui.tableWidget_eyeboxs->rowCount();
    for (int i = 0; i < rowAll; ++i)
    {
        ui.tableWidget_eyeboxs->setRowHidden(i, false);
    }

    for (int i = rowCount; i < rowAll; ++i)
    {
        ui.tableWidget_eyeboxs->setRowHidden(i, true);
    }
}

Result DutEyeboxWidget::judgetUiData()
{
    int rowCount = ui.comboBox_eyeboxNumber->currentText().toInt (); 
    for (int row = 0; row < rowCount - 1; ++row)
    {
        cv::Point3f value;
        value.x = ui.tableWidget_eyeboxs->item(row, 0)->text().toFloat();
        value.y = ui.tableWidget_eyeboxs->item(row, 1)->text().toFloat();
        value.z = ui.tableWidget_eyeboxs->item(row, 2)->text().toFloat();

        for (int next = row + 1; next < rowCount; ++next)
        {
            cv::Point3f valueNext;
            valueNext.x = ui.tableWidget_eyeboxs->item(next, 0)->text().toFloat();
            valueNext.y = ui.tableWidget_eyeboxs->item(next, 1)->text().toFloat();
            valueNext.z = ui.tableWidget_eyeboxs->item(next, 2)->text().toFloat();

            if (abs(value.x - valueNext.x) < 0.01 && 
                abs(value.y - valueNext.y) < 0.01 && 
                abs(value.y - valueNext.y) < 0.01)
            {
                return Result(false, QString("Exist same data, row %1 and %2.").arg(row + 1).arg(next + 1).toStdString());
            }
        }
    }

    return Result();
}
