#include "FiducialConfigWidget.h"
#include "CustomSpinboxDelegate.h"
#include "CaptureFiducialImage.h"
#include <QMessageBox>
#include <qcompleter.h>
#include "AlignUtils.h"

#include "Core/icore.h"
#include "piMotionActor/FiducialConfig.h"
#include "PrjCommon/PrjCommon.h"

FiducialConfigWidget::FiducialConfigWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    init();
    initTableStyle();
    initTable();
    initMvFiducial();
    connect(ui.radioButton_add, SIGNAL(toggled(bool)), this, SLOT(checkAddSlot(bool)));
    connect(ui.lineEdit_sn, SIGNAL(returnPressed()), this, SLOT(judgeDutInfo()));
}

FiducialConfigWidget::~FiducialConfigWidget()
{
    if (m_eyeboxsWidget != nullptr){
        delete m_eyeboxsWidget;
        m_eyeboxsWidget = nullptr;
    }
}

void FiducialConfigWidget::init()
{
    ui.label_mvFiducialCoord_1->clear();
    ui.label_mvFiducialCoord_2->clear();
    ui.label_mvFiducialCoord_3->clear();

    ui.radioButton_update->setChecked(true);

    radioChange(false);

    ui.cbx_sn->clear();

    ui.groupBox_calcFiducial->hide();

    ui.doubleSpinBox_EyeRelief->setDecimals(DECIMAL);
    ui.doubleSpinBox_EyeOffset->setDecimals(DECIMAL);

    ui.label_17->hide();
    ui.doubleSpinBox_EyeOffset->hide();
    //ui.widget_7->hide();
    ui.label_eyeboxs->setWordWrap(true);
    m_eyeboxsWidget = new DutEyeboxWidget(this);

    ui.btn_exportCsv->hide();
    ui.btn_importCsv->hide();

    ui.comboBox_eyeType->setCurrentIndex(1);
    ui.comboBox_eyeType->setEnabled(false);
}

void FiducialConfigWidget::initTableStyle()
{
    QString qssTV = "QTableWidget::item:hover{background-color:rgb(92,188,227,200)}"
                    "QTableWidget::item:selected{background-color:#1B89A1}"
                    "QHeaderView::section,QTableCornerButton:section{ \
                     padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
                     border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                     background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                    "QTableWidget{background-color:white;border:none;}";

    ui.tableWidget_Fiducial->horizontalHeader()->setFont(QFont("song", 12));
    QFont font = ui.tableWidget_Fiducial->horizontalHeader()->font();
    font.setBold(true);
    ui.tableWidget_Fiducial->horizontalHeader()->setFont(font);

    ui.tableWidget_Fiducial->setStyleSheet(qssTV);

    setWindowTitle("DUT Information Config");
    this->setWindowFlags(Qt::Dialog);
}

void FiducialConfigWidget::initTable()
{
    QStringList HStrList;
    HStrList << "X"
             << "Y"
             << "Z";
    ui.tableWidget_Fiducial->setColumnCount(HStrList.count());
    ui.tableWidget_Fiducial->setHorizontalHeaderLabels(HStrList);

    QStringList VStrList;
    if(FIDUCIAL_COUNT == 2){
        VStrList << "Fiducial1" << "Fiducial2"
            << "EyeboxCenter" << "InputGrating";
    }else if(FIDUCIAL_COUNT == 3) {
        VStrList << "Fiducial1" << "Fiducial2" << "Fiducial3"
            << "EyeboxCenter" << "InputGrating";
    }

    ui.tableWidget_Fiducial->setRowCount(VStrList.count());
    ui.tableWidget_Fiducial->setVerticalHeaderLabels(VStrList);

    initTableStyle();
    QDoubleValidator *val = new QDoubleValidator(this);
    val->setBottom(-10000.000);
    val->setDecimals(DECIMAL);
    val->setTop(10000.000);
    val->setNotation(QDoubleValidator::StandardNotation);

    //匹配正数、负数、浮点数
    //^-?[1-9]\d*\.\d+$|^-?0\.\d+$|^-?[1-9]\d*$|^0$
    //^-?[0-9]*$
    QRegExp regexp("^-?[0-9]*$|^-?[0-9]+(.[0-9]{3})?$");
    //^-?([1-9]\d*\.\d*|0\.\d*[1-9]\d*|0?\.0+|0)$

    int rowCount = ui.tableWidget_Fiducial->rowCount();
    int colCount = ui.tableWidget_Fiducial->columnCount();
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            QTableWidgetItem *xItem = new QTableWidgetItem("0");
            ui.tableWidget_Fiducial->setItem(row, col, xItem);
        }
    }
    ui.tableWidget_Fiducial->setItemDelegate(new CustomSpinboxDelegate());
}

void FiducialConfigWidget::initMvFiducial()
{
    DutConfig dutConfig = CalibrationConfig::instance()->getDutConfig();
    cv::Point3f igCoord = dutConfig.mvRelativeDutInputGrating;
    ui.doubleSpinBox_mvIgX->setValue(igCoord.x);
    ui.doubleSpinBox_mvIgY->setValue(igCoord.y);
    ui.doubleSpinBox_mvIgZ->setValue(igCoord.z);

    if (dutConfig.fiducials.size() < 3){
        ui.label_mvFiducialCoord_1->setText("");
        ui.label_mvFiducialCoord_2->setText("");
        ui.label_mvFiducialCoord_3->setText("");
        return;
    }

    refreshMvFiducial(dutConfig.fiducials);
}

void FiducialConfigWidget::handleMode()
{
    int rowCount = ui.tableWidget_Fiducial->rowCount();
    int colCount = ui.tableWidget_Fiducial->columnCount();
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            ui.tableWidget_Fiducial->item(row, col)->setText("0");
        }
    }

    if (ui.radioButton_add->isChecked())
    {
        radioChange(true);
        ui.lineEdit_sn->setText("");
        ui.label_mvFiducialCoord_1->clear();
        ui.label_mvFiducialCoord_2->clear();
        ui.label_mvFiducialCoord_3->clear();

        ui.checkbox_defaultDut->setEnabled(true);

        ui.label_eyeboxs->setText("0, 0, 0");
        m_eyeboxs.clear();
        m_eyeboxs.push_back(cv::Point3f(0,0,0));

        ui.btn_delete->setHidden(true);
    }
    else
    {
        radioChange(false);

        //ui.tableWidget_Fiducial->setItemDelegate(new CustomSpinboxDelegate());
        ui.cbx_sn->setCurrentText(CalibrationConfig::instance()->getDefaultDutName());
        ui.lineEdit_sn->setText(ui.cbx_sn->currentText());
        updateConfigs();

        ui.btn_delete->setHidden(false);
    }
}

void FiducialConfigWidget::refreshMvFiducial(const std::vector<cv::Point3f> &fs)
{
    if (fs.size() < 3){
        return;
    }

    QString f1 = QString("Fiducial_1 (%1,%2,%3)")
                     .arg(QString::number(fs.at(0).x, 'f', DECIMAL))
                     .arg(QString::number(fs.at(0).y, 'f', DECIMAL))
                     .arg(QString::number(fs.at(0).z, 'f', DECIMAL));
    QString f2 = QString("Fiducial_2 (%1,%2,%3)")
                     .arg(QString::number(fs.at(1).x, 'f', DECIMAL))
                     .arg(QString::number(fs.at(1).y, 'f', DECIMAL))
                     .arg(QString::number(fs.at(1).z, 'f', DECIMAL));
    QString f3 = QString("Fiducial_3 (%1,%2,%3)")
                     .arg(QString::number(fs.at(2).x, 'f', DECIMAL))
                     .arg(QString::number(fs.at(2).y, 'f', DECIMAL))
                     .arg(QString::number(fs.at(2).z, 'f', DECIMAL));
    ui.label_mvFiducialCoord_1->setText(f1);
    ui.label_mvFiducialCoord_2->setText(f2);
    ui.label_mvFiducialCoord_3->setText(f3);
}

void FiducialConfigWidget::loadFiducialConfig(QString sn)
{
    CalibrationConfig::instance()->load();
    DutConfig dutConfig = CalibrationConfig::instance()->loadDutConfig(sn);
    for (int row = 0; row < dutConfig.fiducials.size(); row++)
    {
        ui.tableWidget_Fiducial->item(row, 0)->setText(QString::number(dutConfig.fiducials[row].x, 'f', DECIMAL));
        ui.tableWidget_Fiducial->item(row, 1)->setText(QString::number(dutConfig.fiducials[row].y, 'f', DECIMAL));
        ui.tableWidget_Fiducial->item(row, 2)->setText(QString::number(dutConfig.fiducials[row].z, 'f', DECIMAL));
    }

    int rowNext = FIDUCIAL_COUNT;
    ui.tableWidget_Fiducial->item(rowNext, 0)->setText(QString::number(dutConfig.eyeboxCenter.x, 'f', DECIMAL));
    ui.tableWidget_Fiducial->item(rowNext, 1)->setText(QString::number(dutConfig.eyeboxCenter.y, 'f', DECIMAL));
    ui.tableWidget_Fiducial->item(rowNext, 2)->setText(QString::number(dutConfig.eyeboxCenter.z, 'f', DECIMAL));

    rowNext += 1;
    ui.tableWidget_Fiducial->item(rowNext, 0)->setText(QString::number(dutConfig.inputGrating.x, 'f', DECIMAL));
    ui.tableWidget_Fiducial->item(rowNext, 1)->setText(QString::number(dutConfig.inputGrating.y, 'f', DECIMAL));
    ui.tableWidget_Fiducial->item(rowNext, 2)->setText(QString::number(dutConfig.inputGrating.z, 'f', DECIMAL));

    ui.doubleSpinBox_EyeRelief->setValue(dutConfig.eyeRelief);
    QStringList offsetList = QString::fromStdString(dutConfig.exeboxOffset).split(',');
    if (offsetList.size() > 0)
        ui.doubleSpinBox_EyeOffset->setValue(offsetList[0].toFloat());

    ui.comboBox_eyeType->setCurrentIndex(dutConfig.eyeType);
    ui.checkbox_defaultDut->setChecked(CalibrationConfig::instance()->getDefaultDutName() == dutConfig.sn);
    ui.checkbox_defaultDut->setEnabled(!ui.checkbox_defaultDut->isChecked());

    refreshMvFiducial(dutConfig.motPosOfFids);

    m_eyeboxs = dutConfig.exeboxOffsetAll;
    if (m_eyeboxs.size() == 0)
    {
        m_eyeboxs.push_back(cv::Point3f(0, 0, 0));
    }
    ui.label_eyeboxs->setText(listToStr(m_eyeboxs));
}

DutConfig FiducialConfigWidget::getUIData()
{
    DutConfig config;
    if (ui.radioButton_update->isChecked()){
        config = CalibrationConfig::instance()->getDutConfig(ui.cbx_sn->currentText());
        config.sn = ui.lineEdit_sn->isVisible() ? ui.lineEdit_sn->text().trimmed() : ui.cbx_sn->currentText().trimmed();
    }else{
        config.sn = ui.lineEdit_sn->text().trimmed();
    }

    cv::Point3f f1 = cv::Point3f(ui.tableWidget_Fiducial->item(0, 0)->text().toFloat(),
                                 ui.tableWidget_Fiducial->item(0, 1)->text().toFloat(),
                                 ui.tableWidget_Fiducial->item(0, 2)->text().toFloat());
    cv::Point3f f2 = cv::Point3f(ui.tableWidget_Fiducial->item(1, 0)->text().toFloat(),
                                 ui.tableWidget_Fiducial->item(1, 1)->text().toFloat(),
                                 ui.tableWidget_Fiducial->item(1, 2)->text().toFloat());
    cv::Point3f f3;
    if(FIDUCIAL_COUNT == 3){
        f3 = cv::Point3f(ui.tableWidget_Fiducial->item(2, 0)->text().toFloat(),
            ui.tableWidget_Fiducial->item(2, 1)->text().toFloat(),
            ui.tableWidget_Fiducial->item(2, 2)->text().toFloat());
    }

    int rowNext = FIDUCIAL_COUNT;
    cv::Point3f center = cv::Point3f(ui.tableWidget_Fiducial->item(rowNext, 0)->text().toFloat(),
                                     ui.tableWidget_Fiducial->item(rowNext, 1)->text().toFloat(),
                                     ui.tableWidget_Fiducial->item(rowNext, 2)->text().toFloat());
    rowNext += 1;
    cv::Point3f inputGrating = cv::Point3f(ui.tableWidget_Fiducial->item(rowNext, 0)->text().toFloat(),
                                           ui.tableWidget_Fiducial->item(rowNext, 1)->text().toFloat(),
                                           ui.tableWidget_Fiducial->item(rowNext, 2)->text().toFloat());
    cv::Point3f mvRelativeDutIG = cv::Point3f(ui.doubleSpinBox_mvIgX->value(), 
                                              ui.doubleSpinBox_mvIgY->value(), 
                                              ui.doubleSpinBox_mvIgZ->value());

    config.fiducials.clear();
    config.fiducials.push_back(f1);
    config.fiducials.push_back(f2);
    if (FIDUCIAL_COUNT == 3) {
        config.fiducials.push_back(f3);
    }

    config.inputGrating = inputGrating;
    config.eyeboxCenter = center;
    config.eyeRelief = ui.doubleSpinBox_EyeRelief->value();
    config.exeboxOffset = ui.doubleSpinBox_EyeOffset->text().toStdString();
    config.mvRelativeDutInputGrating = mvRelativeDutIG;
    config.calculateMvFidsByInputFlag = ui.checkBox_cailFids->isChecked();
    config.eyeType = ui.comboBox_eyeType->currentIndex();

    if (m_eyeboxs.size() == 0){
        m_eyeboxs.push_back(cv::Point3f(0,0,0));
    }
    config.exeboxOffsetAll = m_eyeboxs;
    return config;
}

Result FiducialConfigWidget::updateConfigs()
{
    ui.cbx_sn->clear();

    QList<QString> snList = CalibrationConfig::instance()->getDutSns();
    for (int i = 0; i < snList.size(); i++)
        ui.cbx_sn->addItem(snList[i]);

    QString sn = CalibrationConfig::instance()->getDefaultDutName();
    ui.cbx_sn->setCurrentText(sn);
    ui.lineEdit_sn->setText(sn);

    loadFiducialConfig(sn);
    return Result();
}

Result FiducialConfigWidget::addConfigs()
{
    return Result();
}

void FiducialConfigWidget::radioChange(bool isAdd)
{
    ui.lineEdit_sn->setVisible(isAdd);
    ui.cbx_sn->setVisible(!isAdd);
    ui.btn_snEdit->setVisible(!isAdd);

    ui.btn_snEdit->setText("Edit Model");
}

void FiducialConfigWidget::updateUIBySave()
{
    if(ui.radioButton_add->isChecked()){
        return;
    }

    radioChange(false);
    if(ui.lineEdit_sn->text() == ui.cbx_sn->currentText()){
        return;
    }

    ui.cbx_sn->clear();
    QList<QString> snList = CalibrationConfig::instance()->getDutSns();
    for (int i = 0; i < snList.size(); i++)
        ui.cbx_sn->addItem(snList[i]);

    QString sn = ui.lineEdit_sn->text();
    ui.cbx_sn->setCurrentText(sn);
    loadFiducialConfig(sn);
}

QString FiducialConfigWidget::listToStr(std::vector<cv::Point3f> list)
{
    QString text;
    for (int i = 0; i < list.size(); ++i){
        cv::Point3f point = list.at(i);
        QString pointStr = QString("%1,%2,%3").arg(point.x).arg(point.y).arg(point.z);
        if(i == 0){
            text = pointStr;
        }else{
            text = text + ";  " + pointStr;
        }
    }
    return text;
}

Result FiducialConfigWidget::updateFiducialConfig(const DutConfig &oldData, const DutConfig &newData)
{
    bool snSame = (oldData.sn == newData.sn);
    bool posSame = fiducialAndInputRelPosChange(oldData, newData);

    if (snSame)
    {
        if (!posSame)
        {
        
        }
    }
    else
    {
        if (posSame)
        {

        }
        else
        {
        
        }
    }
    return Result();
}

bool FiducialConfigWidget::fiducialAndInputRelPosChange(const DutConfig &oldData, const DutConfig &newData)
{
    if (oldData.fiducials.size() != 3 || newData.fiducials.size() != 3)
    {
        return false;
    }

    cv::Point3f oldInF1 = oldData.inputGrating - oldData.fiducials[0];
    cv::Point3f oldInF2 = oldData.inputGrating - oldData.fiducials[1];
    cv::Point3f oldInF3 = oldData.inputGrating - oldData.fiducials[2];

    cv::Point3f newInF1 = newData.inputGrating - newData.fiducials[0];
    cv::Point3f newInF2 = newData.inputGrating - newData.fiducials[1];
    cv::Point3f newInF3 = newData.inputGrating - newData.fiducials[2];

    if (abs(oldInF1.x - newInF1.x) > 0.1 || abs(oldInF1.y - newInF1.y) > 0.1 || abs(oldInF1.z - newInF1.z) > 0.1)
    {
        return true;
    }

    if (abs(oldInF2.x - newInF2.x) > 0.1 || abs(oldInF2.y - newInF2.y) > 0.1 || abs(oldInF2.z - newInF2.z) > 0.1)
    {
        return true;
    }

    if (abs(oldInF3.x - newInF3.x) > 0.1 || abs(oldInF3.y - newInF3.y) > 0.1 || abs(oldInF3.z - newInF3.z) > 0.1)
    {
        return true;
    }
    return false;
}

void FiducialConfigWidget::checkAddSlot(bool isAdd)
{
    if (isAdd){
        ui.checkbox_defaultDut->setChecked(true);
    }
    handleMode();
}

bool FiducialConfigWidget::judgeDutInfo()
{
    QString sn;
    QList<QString> sblist = CalibrationConfig::instance()->getDutSns();
    if (ui.radioButton_update->isChecked())
    {
        sblist.removeOne(ui.cbx_sn->currentText());
        sn = ui.lineEdit_sn->isVisible() ? ui.lineEdit_sn->text().trimmed() : ui.cbx_sn->currentText();
    }else{
        ui.lineEdit_sn->clearFocus();
        sn = ui.lineEdit_sn->text().trimmed();
    }

    if (sblist.contains(sn))
    {
        QMessageBox::warning(this, "Warning!", "DUT SN already exists, please reset it!");
        ui.radioButton_add->isChecked() ? ui.lineEdit_sn->setFocus() : ui.cbx_sn->setFocus();
        return false;
    }
    else if (sn == "")
    {
        QMessageBox::warning(this, "Warning!", "Please input the DUT SN!");
        ui.radioButton_add->isChecked() ? ui.lineEdit_sn->setFocus() : ui.cbx_sn->setFocus();
        return false;
    }

    return true;
}

bool FiducialConfigWidget::checkItemIsNull()
{
    int rowCount = ui.tableWidget_Fiducial->rowCount();
    int colCount = ui.tableWidget_Fiducial->columnCount();
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            if (ui.tableWidget_Fiducial->item(row, col)->text().isEmpty())
            {
                return true;
            }
        }
        return false;
    }
}

void FiducialConfigWidget::on_btn_eyeboxs_clicked()
{
    m_eyeboxsWidget->updateEyeboxs(m_eyeboxs);
    int result = m_eyeboxsWidget->exec();
    if (result == QDialog::Accepted)
    {
        m_eyeboxs = m_eyeboxsWidget->getEyeboxs();
        ui.label_eyeboxs->setText(listToStr(m_eyeboxs));
    } 
}

void FiducialConfigWidget::on_btn_importCsv_clicked()
{
}

void FiducialConfigWidget::on_btnSaveDefaultDut_clicked()
{
    ui.checkbox_defaultDut->setChecked(true);
    on_btnSave_clicked();
}

void FiducialConfigWidget::on_btn_delete_clicked()
{
    if(ui.checkbox_defaultDut->isChecked() && !ui.checkbox_defaultDut->isEnabled()){
        QMessageBox::warning(this, "Delete DUT Warning!", "Forbidden to delete the default test DUT.");
        return;
    }

    QString sn = ui.cbx_sn->currentText();
    QMessageBox::StandardButton box = QMessageBox::question(this, "Question!", QString("Are you sure to delete current DUT %1 config?").arg(sn));
    if (box == QMessageBox::No)
    {
        return;
    }

    Result ret = CalibrationConfig::instance()->deleteDutConfig(sn);
    if (!ret.success)
    {
        QMessageBox::critical(this, "Delete DUT Error!", QString::fromStdString(ret.errorMsg));
    }else{
        handleMode();
        QMessageBox::information(this, "Delete DUT", QString("Delete DUT %1 config success!").arg(sn));
    }
}

void FiducialConfigWidget::on_btn_calcMvFiducial_clicked()
{
    DutConfig config = getUIData();
    AlignUtils::dutMcSameDirection(config);

    vector<cv::Point3f> fiducials;
    Result ret = CaptureFiducialImage().calMvCaptureFiducialCoords2D(fiducials, config);
    if(!ret.success){
        QMessageBox::critical(this, "Calculate MV fiducial", QString::fromStdString(ret.errorMsg),
                              QMessageBox::Ok, QMessageBox::NoButton);
        m_mvFiducials.clear();
        return;
    }
    m_mvFiducials = fiducials;
    refreshMvFiducial(fiducials);
}

void FiducialConfigWidget::on_comboBox_eyeType_activated(int index)
{
    cv::Point3f cfg= CalibrationConfig::instance()->loadEyeTypeConfig((CORE::DUT_EYE_TYPE)index);
    ui.doubleSpinBox_mvIgX->setValue(cfg.x);
    ui.doubleSpinBox_mvIgY->setValue(cfg.y);
    ui.doubleSpinBox_mvIgZ->setValue(cfg.z);
}

//need to do focus in/out
bool FiducialConfigWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        QMessageBox::StandardButton box;
        box =
            QMessageBox::warning(this, "Warning!",
                                 "The DUT SN have existed!Please re-enter text or toggle to this DUT showing pages by "
                                 "choosing 'update DUT'!");
        if (box == QMessageBox::Ok)
            ui.lineEdit_sn->setFocus();
        ui.lineEdit_sn->setText("");
    }
    return QWidget::eventFilter(watched, event);
}
void FiducialConfigWidget::on_cbx_sn_activated(int index)
{
    ui.label_mvFiducialCoord_1->clear();
    ui.label_mvFiducialCoord_2->clear();
    ui.label_mvFiducialCoord_3->clear();

    QString sn = ui.cbx_sn->currentText();
    ui.lineEdit_sn->setText(sn);
    loadFiducialConfig(sn);
}

void FiducialConfigWidget::on_btn_snEdit_clicked()
{
   if(ui.btn_snEdit->text() == "Edit Model"){
       ui.btn_snEdit->setText("Cancel Edit");
       ui.lineEdit_sn->setHidden(false);
       ui.cbx_sn->setHidden(true);
   }else{
       ui.btn_snEdit->setText("Edit Model");
       ui.lineEdit_sn->setHidden(true);
       ui.cbx_sn->setHidden(false);

       ui.lineEdit_sn->setText(ui.cbx_sn->currentText());
   }
}

void FiducialConfigWidget::showEvent(QShowEvent *)
{
    QList<QString> snList = CalibrationConfig::instance()->getDutSns();
    if (snList.size() > 0)
    {
        ui.radioButton_update->setChecked(true);
    }
    else
    {
        ui.radioButton_add->setChecked(true);
    }

    handleMode();
}

void FiducialConfigWidget::on_btnSave_clicked()
{    
    bool ret = judgeDutInfo();
    if (!ret)
    {
        return;
    }

    ret = checkItemIsNull();
    if (ret)
    {
        QMessageBox::critical(this, "Save Config",
                              "Get DUT config error,please check the integrity of input chart data.", QMessageBox::Ok,
                              QMessageBox::NoButton);
        return;
    }

    DutConfig config = getUIData();
    ret= CalibrationConfig::instance()->writeDutConfig(config);
    if(!ret){
        QMessageBox::critical(this, "Save Config", "Save DUT config error.", QMessageBox::Ok, QMessageBox::NoButton);
        return;
    }
    if (m_mvFiducials.size() >= 3){
        ret = CalibrationConfig::instance()->writeMotPosOfFids(config.sn, m_mvFiducials);
        if (!ret)
        {
            emit UpdateConfigSignal();
            emit Core::PrjCommon::instance()->updateDutConfigSignal();
            QMessageBox::critical(this, "Save Config", "Save MV fiducial config error.", QMessageBox::Ok,
                                  QMessageBox::NoButton);
            return;
        }
    }
    if (ui.radioButton_add->isChecked())
    {
        ret = CalibrationConfig::instance()->addSnName(ui.lineEdit_sn->text().trimmed());
        if (!ret)
        {
            QMessageBox::critical(this, "Save Config", "Save DUT Base config error.", QMessageBox::Ok,
                                  QMessageBox::NoButton);
            return;
        }
    }else{
        CalibrationConfig::instance()->updateSnName(config.sn, ui.cbx_sn->currentText());
        if (!ret)
        {
            QMessageBox::critical(this, "Save Config", "Save DUT Base config error.", QMessageBox::Ok,
                                  QMessageBox::NoButton);
            return;
        }
    }

    if (ui.checkbox_defaultDut->isChecked())
    {
        Result result = CalibrationConfig::instance()->setDefaultDutName(config.sn);
        if (!result.success)
        {
            QMessageBox::critical(this, "Save Config",
                                  "Save Default Dut Name config error, " + QString::fromStdString(result.errorMsg),
                                  QMessageBox::Ok,
                                  QMessageBox::NoButton);
            return;
        }
    }
    emit UpdateConfigSignal();
    emit Core::PrjCommon::instance()->updateDutConfigSignal();

    updateUIBySave();
    ui.checkbox_defaultDut->setEnabled(!ui.checkbox_defaultDut->isChecked());

    QMessageBox::information(this, "Save Config", "Save success!", QMessageBox::Ok, QMessageBox::NoButton);
}
