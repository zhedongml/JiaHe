#pragma once

#include <QWidget>
#include "ui_dutPrCalibrateWidget.h"
#include <piMotionActor/DutProjectorCalibrate.h>

class dutPrCalibrateWidget : public QWidget
{
	Q_OBJECT

public:
	dutPrCalibrateWidget(QWidget *parent = nullptr);
	~dutPrCalibrateWidget();
private:
    void initTableStyle();
    void initTable();
    Result updateTable();
    void loadCalibrateInfo();
	DutPrNearInfo getCurrentPos();
    Result judgeConnectStatus();
  private slots:
    void on_btn_save1_clicked();
    void on_btn_refresh_clicked();
    void on_btn_setLight_clicked();
    void updateDutConfigSlot();

    void on_btn_delete_clicked();

  private:
	Ui::dutPrCalibrateWidgetClass ui;
  const float UNIT_CONVERSION_T = 1000.0;
    QMap<DUT_EYE_TYPE, QList<DutPrNearInfo>> m_dutPrNearInfo;
  QStringList m_VStrList;
    QStringList m_HStrList;
    QMap<int, int> m_rowIdMap;
};
