#include "dutPrCalibrateWidget.h"
#include <PrjCommon/service/ml.h>
#include <piMotionActor/Motion2DModel.h>
#include <piMotionActor/Motion3DModel.h>
#include "CustomSpinboxDelegate.h"
#include <QtWidgets/qmessagebox.h>
#include "piMotionActor/calibrationconfig.h"
#include <Core/icore.h>
#include "AlignUtils.h"
#include "PrjCommon/PrjCommon.h"

dutPrCalibrateWidget::dutPrCalibrateWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    DutProjectorCalibrate::instance()->createDutPrNearCalibrateTable();

    initTableStyle();
    initTable();
    on_btn_refresh_clicked();

    QPixmap* pixmap=new QPixmap(":/image/image/input_left.png");
    pixmap->scaled(ui.label_3->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui.label_3->setScaledContents(true);
    ui.label_3->setPixmap(*pixmap);

    QPixmap *pixmapT = new QPixmap(":/image/image/input_right.png");
    pixmapT->scaled(ui.label_2->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui.label_2->setScaledContents(true);
    ui.label_2->setPixmap(*pixmapT);

    connect(Core::PrjCommon::instance(), SIGNAL(updateDutConfigSignal()), this, SLOT(updateDutConfigSlot()));
    updateDutConfigSlot();

    {
        ui.label_instructions->setWordWrap(true);
        ui.label_instructions->setText("Operating instructions:\n\n"
            "1. View MV image in the \"InputAA\" module.\n\n"
            "2. DUT approaches the Projector in the Z direction.\n\n"
            "3. Make sure that when saving the configuration, the motion stage should be positioned in such a way that the imaging results are as shown in the image on the right.");
    }

    //ui.btn_delete->hide();
    ui.eyetype->setEnabled(false);
}

dutPrCalibrateWidget::~dutPrCalibrateWidget()
{
}
void dutPrCalibrateWidget::initTableStyle()
{
    QString qssTV = "QTableWidget::item:hover{background-color:rgb(92,188,227,200)}"
                    /*"QTableWidget::item {height:50px;width:110px;}"*/
                    "QTableWidget::item:selected{background-color:#1B89A1}"
                    "QHeaderView::section,QTableCornerButton:section{ \
                     padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
                     border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                     background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                    "QTableWidget{background-color:white;border:none;}";

    ui.tableWidget->horizontalHeader()->setFont(QFont("song", 10));

    QFont font = ui.tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui.tableWidget->horizontalHeader()->setFont(font);

    ui.tableWidget->setStyleSheet(qssTV);

    ui.tableWidget->setColumnWidth(5, 180);
    ui.tableWidget->setColumnWidth(6, 180);
    ui.tableWidget->setColumnWidth(7, 180);

}

//void dutPrCalibrateWidget::paintEvent(QPaintEvent *event)
//{
//    
//    QPainter p(ui.label);
//    QImageReader reader;
//    QImage image;
//    reader.setDecideFormatFromContent(true);
//    reader.setScaledSize(QSize(width(), height()));
//    reader.setFileName(":/image/image/input.png");
//    if (reader.canRead())
//    {
//        if (!reader.read(&image))
//        {
//            QImageReader::ImageReaderError error = reader.error();
//            QString strError = reader.errorString();
//            printf("last error:%s\n", strError.toStdString().c_str());
//            return;
//        }
//    }
//    p.drawPixmap(0, 0, ui.label->width(), ui.label->height(),
//                 QPixmap::fromImage(image) /*.scaled(width(), height(), Qt::KeepAspectRatio)*/);
//}

void dutPrCalibrateWidget::initTable()
{
    m_HStrList << "EyeType"
             << "DutX"
             << "DutY"
             << "ProjectorX"
             << "ProjectorY"
             << "Dut3DCoordinateX"
             << "Dut3DCoordinateY"
             << "Dut3DCoordinateZ";
    ui.tableWidget->setColumnCount(m_HStrList.count());
    ui.tableWidget->setHorizontalHeaderLabels(m_HStrList);
    updateTable();
    initTableStyle();
    //ui.tableWidget->setItemDelegate(new CustomSpinboxDelegate());
}
Result dutPrCalibrateWidget::updateTable()
{
    m_VStrList.clear();
    Result ret = DutProjectorCalibrate::instance()->queryDatas(m_dutPrNearInfo);
    if (!ret.success)
    {
        return ret;
    }

    if (!m_dutPrNearInfo.isEmpty())
    {
        int left = m_dutPrNearInfo.value((DUT_EYE_TYPE)0).size();
        int right = m_dutPrNearInfo.value((DUT_EYE_TYPE)1).size();
        int ksize = left + right;
        for (int i = 0; i < ksize; i++)
            m_VStrList.append(QString::number(i));
    }

    ui.tableWidget->setRowCount(m_VStrList.count());
    ui.tableWidget->setVerticalHeaderLabels(m_VStrList);

    for (int row = 0; row < m_VStrList.size(); ++row)
    {
        for (int col = 0; col < m_HStrList.size(); ++col)
        {
            QTableWidgetItem *xItem = new QTableWidgetItem("");
            ui.tableWidget->setItem(row, col, xItem);
        }
    }
    return Result();
}
void dutPrCalibrateWidget::on_btn_save1_clicked()
{
    Result res = judgeConnectStatus();
    if (!res.success)
    {
        QMessageBox::warning(NULL, "Warning!",QString::fromStdString(res.errorMsg), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    DutPrNearInfo poss = getCurrentPos();
    //to test:
    //poss.dutAngle = cv::Point2f(4, -9);
    //poss.prAngle = cv::Point2f(8, -9);
    //poss.dutPrNearCoordinate = cv::Point3f(-7,1, 2);

    int index = ui.eyetype->currentIndex();
    QMessageBox::StandardButton box = QMessageBox::question(this, "Question!",
                               "Have you confirmed that the current InputAA scene is as same as shown in the image above, and want to save the current location information?");
    if (box == QMessageBox::Yes)
    {
        Result ret = DutProjectorCalibrate::instance()->updateDutPrNearCalibrateTableValue(poss.dutAngle, poss.prAngle, index,
                                                                              poss.dutPrNearCoordinate);
        if (!ret.success)
        {
            QMessageBox::warning(NULL, "Warning!", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }
    else
    {
        return;
    }
    on_btn_refresh_clicked();

    QMessageBox::information(NULL, "Save", "Save success!", QMessageBox::Ok, QMessageBox::Ok);
}

void dutPrCalibrateWidget::on_btn_refresh_clicked()
{
    Result ret = updateTable();
    if (!ret.success)
    {
        QMessageBox::warning(NULL, "Warning!", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    loadCalibrateInfo();
}
void dutPrCalibrateWidget::on_btn_setLight_clicked()
{
    Result ret = AlignUtils::controlLight(true, true);
    if (!ret.success)
    {
        QMessageBox::warning(NULL, "Warning!", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
}
void dutPrCalibrateWidget::updateDutConfigSlot()
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    ui.eyetype->setCurrentIndex(dutConfig.eyeType);
}
void dutPrCalibrateWidget::on_btn_delete_clicked()
{
    int row = ui.tableWidget->currentRow();
    QMessageBox::StandardButton box = QMessageBox::question(this, "Question!", QString("Are you sure to delete the record in line %1 ?").arg(row));
    if (box == QMessageBox::No)
    {
        return;
    }

    if(m_rowIdMap.contains(row)){
        Result ret = DutProjectorCalibrate::instance()->deleteRecord(m_rowIdMap[row]);
        if (!ret.success)
        {
            QMessageBox::warning(NULL, "Warning!", QString::fromStdString(ret.errorMsg), QMessageBox::Ok,
                                 QMessageBox::Ok);
            return;
        }
    }

    on_btn_refresh_clicked();
    QMessageBox::information(NULL, "Delete", "Delete success!", QMessageBox::Ok, QMessageBox::Ok);
}
void dutPrCalibrateWidget::loadCalibrateInfo()
{
    m_rowIdMap.clear();
    DutProjectorCalibrate::instance()->queryDatas(m_dutPrNearInfo);

    int infos = m_dutPrNearInfo.size();
    QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>>::iterator it;
    int row = 0;
    QString eye;
    int allSize;
    for (int i = 0; i < 2; i++)
    {
        if (!i)
        {
             allSize = m_dutPrNearInfo.value((DUT_EYE_TYPE)0).size();
            eye = "left";
        }
        else if (i == 1)
        {
            allSize = m_dutPrNearInfo.value((DUT_EYE_TYPE)1).size();
            eye = "right";
        }
        int index = 0;
        for (auto it: m_dutPrNearInfo.value((DUT_EYE_TYPE)i))
        {
            if (index < allSize)
            {
                QTableWidgetItem *item = ui.tableWidget->item(row, 0);
                ui.tableWidget->item(row, 0)->setText(eye);
                ui.tableWidget->item(row, 1)->setText(QString::number(it.dutAngle.x, 'f', 2));
                ui.tableWidget->item(row, 2)->setText(QString::number(it.dutAngle.y, 'f', 2));
                ui.tableWidget->item(row, 3)->setText(QString::number(it.prAngle.x, 'f', 2));
                ui.tableWidget->item(row, 4)->setText(QString::number(it.prAngle.y, 'f', 2));
                ui.tableWidget->item(row, 5)->setText(QString::number(it.dutPrNearCoordinate.x, 'f', 2));
                ui.tableWidget->item(row, 6)->setText(QString::number(it.dutPrNearCoordinate.y, 'f', 2));
                ui.tableWidget->item(row, 7)->setText(QString::number(it.dutPrNearCoordinate.z, 'f', 2));
                
                m_rowIdMap[row] = it.id;
                row++;
            }
    }   
    }

}

DutPrNearInfo dutPrCalibrateWidget::getCurrentPos()
{
    DutPrNearInfo info;
    CORE::ML_Point dutCoord = Motion2DModel::getInstance(ACS2DDUT)->getPosition();
    cv::Point2f dutAngle =cv::Point2f(dutCoord.x / UNIT_CONVERSION_T, dutCoord.y / UNIT_CONVERSION_T);
    dutAngle.x = (int)round((dutAngle.x*10))/10.0;
    dutAngle.y = (int)round((dutAngle.y*10))/10.0;

    CORE::ML_Point prCoord = Motion2DModel::getInstance(ACS2DPro)->getPosition();
    cv::Point2f prAngle =cv::Point2f(prCoord.x / UNIT_CONVERSION_T, prCoord.y / UNIT_CONVERSION_T);
    prAngle.x = (int)round((prAngle.x * 10)) / 10.0;
    prAngle.y = (int)round((prAngle.y * 10)) / 10.0;

    CORE::ML_Point3D collideCoord = Motion3DModel::getInstance(withDUT)->getPosition();
    cv::Point3f collidePos = cv::Point3f(collideCoord.x / UNIT_CONVERSION_T,
                                           collideCoord.y / UNIT_CONVERSION_T,
                                         collideCoord.z / UNIT_CONVERSION_T);
    collidePos.x = (int)round((collidePos.x * 10)) / 10.0;
    collidePos.y = (int)round((collidePos.y * 10)) / 10.0;
    collidePos.z = (int)round((collidePos.z * 10)) / 10.0;
    info.dutAngle = dutAngle;
    info.prAngle = prAngle;
    info.dutPrNearCoordinate = collidePos;
    return info;
}

Result dutPrCalibrateWidget::judgeConnectStatus()
{
    std::string msg;
    bool st=Motion2DModel::getInstance(ACS2DDUT)->Motion2DisConnected();
    if (!st)
    {
        msg = "The tilt station with DUT is disconnected, Please check.";
        return Result(st, msg);
    }
    Result re = Motion2DModel::getInstance(ACS2DDUT)->isHoming();
    if (!re.success)
        return re;
    st=Motion2DModel::getInstance(ACS2DPro)->Motion2DisConnected();
    if (!st)
    {
        msg = "The tilt station with Projector is disconnected, Please check.";
        return Result(st, msg);
    }
    st=Motion3DModel::getInstance(withDUT)->Motion3DisConnected();
    if (!st)
    {
        msg = "The move station with DUT is disconnected, Please check.";
        return Result(st, msg);
    }
    return Result();
}
