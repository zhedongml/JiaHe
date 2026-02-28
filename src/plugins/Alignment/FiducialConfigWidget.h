#pragma once

#include "ui_FiducialConfigWidget.h"
#include <QWidget>
#include "piMotionActor/calibrationconfig.h"
#include "DutEyeboxWidget.h"

class FiducialConfigWidget : public QWidget
{
    Q_OBJECT

  public:
    FiducialConfigWidget(QWidget *parent = nullptr);
    ~FiducialConfigWidget();
    void init();
    void initMvFiducial();
    void loadFiducialConfig(QString sn);

  private:
    void initTableStyle();
    void initTable();
    void handleMode();
    void refreshMvFiducial(const std::vector<cv::Point3f> &fiducials);

    DutConfig getUIData();

    Result updateConfigs();
    Result addConfigs();

    void radioChange(bool isAdd);
    void updateUIBySave();
    QString listToStr(std::vector<cv::Point3f> list);

    Result updateFiducialConfig(const DutConfig &oldData, const DutConfig &newData);
    bool fiducialAndInputRelPosChange(const DutConfig &oldData, const DutConfig &newData);

signals:
    void UpdateConfigSignal();
    void AddDocument(bool isNeedAddNew);

private slots:
    void on_btnSave_clicked();
    void on_btnSaveDefaultDut_clicked();
    void on_btn_delete_clicked();
    void on_btn_calcMvFiducial_clicked();
    void on_comboBox_eyeType_activated(int index);  
    bool eventFilter(QObject *watched, QEvent *event);
    void on_cbx_sn_activated(int index);
    void on_btn_snEdit_clicked();
    virtual void showEvent(QShowEvent *) override;
    void checkAddSlot(bool);
    bool judgeDutInfo();
    bool checkItemIsNull();

    void on_btn_eyeboxs_clicked();
    void on_btn_importCsv_clicked();

  private:
    Ui::FiducialConfigWidgetClass ui;
    const int DECIMAL = 3;
    std::vector<cv::Point3f> m_mvFiducials;
    DutEyeboxWidget *m_eyeboxsWidget;
    std::vector<cv::Point3f> m_eyeboxs;
    const int FIDUCIAL_COUNT = 2;
};
