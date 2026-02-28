#include "MinMaxStaWidget.h"

MinMaxStaWidget::MinMaxStaWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint, true);
}

MinMaxStaWidget::~MinMaxStaWidget()
{
}
void MinMaxStaWidget::setAction(QAction *action)
{
    m_action = action;
}
void MinMaxStaWidget::closeEvent(QCloseEvent *event)
{
    if (m_action)
    {
        m_action->setChecked(false);
    }
}
void MinMaxStaWidget::updateMinMaxSta(double minVal, double maxVal, cv::Point minLoc, cv::Point maxLoc)
{
    ui.min_val->setText(QString::number(minVal));
    ui.max_val->setText(QString::number(maxVal));
    ui.min_pos->setText("(" + QString::number(minLoc.x) + "," + QString::number(minLoc.y) + ")");
    ui.max_pos->setText("(" + QString::number(maxLoc.x) + "," + QString::number(maxLoc.y) + ")");
}