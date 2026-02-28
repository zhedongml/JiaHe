#include "LineCutWidget.h"
#include <QDateTime>
#include <QDebug>

LineCutWidget::LineCutWidget(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
    resize(600,350);
    QHBoxLayout *qHBoxLayout = new QHBoxLayout(this);
    plotProfileChart = new CustomPlot(false);
    //plotProfileChart->setTitle("Plot Profile");
    plotProfileChart->xAxis->setLabel("Distance(pixels)");
    plotProfileChart->yAxis->setLabel("Gray Value");
    plotProfileChart->addCurve(QVector<double>(), QVector<double>(), "Plot Profile");
    qHBoxLayout->addWidget(plotProfileChart, 1);

	setWindowIcon(QIcon(":/image/images/logo/mlico1.ico"));
	setWindowFlag(Qt::WindowStaysOnTopHint, true);
    QSettings settings("IQTTester", "ProFile");
    QPoint lastPosition = settings.value("alarmWidgetPosition", QPoint(100, 100)).toPoint();
    move(lastPosition);
}

LineCutWidget::~LineCutWidget()
{
    QSettings settings("IQTTester", "ProFile");
    settings.setValue("alarmWidgetPosition", pos());
}

void LineCutWidget::setAction(QAction *action)
{
    m_action = action;
}

void LineCutWidget::closeEvent(QCloseEvent *event)
{
    QSettings settings("YourOrganization", "LineCutWidget");
    settings.setValue("geometry", saveGeometry());

    QWidget::closeEvent(event);

    if (m_action){
        m_action->setChecked(false);
    }
}

void LineCutWidget::onUpdateCutLine(QList<QPointF> m_CutLineAxisYGrayDatas)
{
    plotProfileChart->clearDatas();
    if (isVisible())
    {
        QVector<double> xV; 
        QVector<double> yV;
        for(QPointF point: m_CutLineAxisYGrayDatas){
            xV.push_back(point.x());
            yV.push_back(point.y());
        }
        plotProfileChart->addDatas(xV, yV);

        qreal minY = std::numeric_limits<qreal>::max();
        qreal maxY = std::numeric_limits<qreal>::lowest();
        qreal minX = std::numeric_limits<qreal>::max();
        qreal maxX = std::numeric_limits<qreal>::lowest();
        for (const auto &point : m_CutLineAxisYGrayDatas)
        {
            minY = std::min(minY, point.y());
            maxY = std::max(maxY, point.y());
            minX = std::min(minX, point.x());
            maxX = std::max(maxX, point.x());
        }
        plotProfileChart->xAxis->setRange(minX, maxX);
        plotProfileChart->yAxis->setRange(minY, maxY);
    }
}
