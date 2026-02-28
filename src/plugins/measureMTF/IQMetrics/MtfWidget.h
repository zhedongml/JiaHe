#pragma once

#include "ui_MtfWidget.h"
#include "MtfDialog.h"
#include "MetricsWidgetBase.h"

class MtfWidget : public MetricsWidgetBase
{
    Q_OBJECT

public:
    MtfWidget(CameraViewer* viewer, QMenu* menu, QWidget* parent = nullptr);
    ~MtfWidget();

private slots:
    void on_btn_calculate_clicked();
    void on_comboBox_fov_activated(int index);
    void radioToggled(bool checked);

protected:
    void initClear() override {}

private:
    void initUI();
    void initAccordion(QAccordion* accordion);
    void initMenu(QMenu* menu);

    QMap<int, cv::Rect> getRois();
    MTF_FOV_TYPE currentFov();
    std::vector<std::string> getTableFreqs();

    Result calculateMetrics();

private:
    Ui::MtfWidgetClass ui;

    MtfDialog* m_mtfDialog;
};
