#include "WaitWidget.h"
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QMutex>

WaitWidget *WaitWidget::m_instance = nullptr;

WaitWidget::WaitWidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Loading . . .");
    initStyle();

    int base = 2;
    m_inRadius = 30 * base;
    m_outRadius = 5 * base;
    //effect 1
    //    m_color[0] = QColor(255,140,0);
    //    m_color[1] = QColor(255,140,0);
    //    m_color[2] = QColor(255,140,0);
    //    m_color[3] = QColor(255,140,0);
    //    m_color[4] = QColor(34,139,34);
    //    m_color[5] = QColor(34,139,34);
    //    m_color[6] = QColor(34,139,34);
    //    m_color[7] = QColor(34,139,34);

    //    m_outRadiuslist[0] = 8;
    //    m_outRadiuslist[1] = 8;
    //    m_outRadiuslist[2] = 8;
    //    m_outRadiuslist[3] = 8;
    //    m_outRadiuslist[4] = 8;
    //    m_outRadiuslist[5] = 8;
    //    m_outRadiuslist[6] = 8;
    //    m_outRadiuslist[7] = 10;

    //effect2
    m_color[0] = QColor(9, 126, 186);
    m_color[1] = QColor(9, 126, 186);
    m_color[2] = QColor(9, 126, 186);
    m_color[3] = QColor(9, 126, 186);
    m_color[4] = QColor(9, 126, 186);
    m_color[5] = QColor(9, 126, 186);
    m_color[6] = QColor(9, 126, 186);
    m_color[7] = QColor(9, 126, 186);

    m_outRadiuslist[0] = 1 * base;
    m_outRadiuslist[1] = 2 * base;
    m_outRadiuslist[2] = 3 * base;
    m_outRadiuslist[3] = 4 * base;
    m_outRadiuslist[4] = 5 * base;
    m_outRadiuslist[5] = 6 * base;
    m_outRadiuslist[6] = 7 * base;
    m_outRadiuslist[7] = 8 * base;

    m_point[0] = QPoint(this->width() / 2 - m_outRadius - m_inRadius,
                        this->height() / 2 - m_outRadius); 
    m_point[1] = QPoint(this->width() / 2 - m_outRadius - m_inRadius * 2 / 3,
                        this->height() / 2 - m_outRadius - m_inRadius * 2 / 3); 
    m_point[2] = QPoint(this->width() / 2 - m_outRadius,
                        this->height() / 2 - m_outRadius - m_inRadius); 
    m_point[3] = QPoint(this->width() / 2 - m_outRadius + m_inRadius * 2 / 3,
                        this->height() / 2 - m_outRadius - m_inRadius * 2 / 3); 
    m_point[4] = QPoint(this->width() / 2 - m_outRadius + m_inRadius,
                        this->height() / 2 - m_outRadius);
    m_point[5] = QPoint(this->width() / 2 - m_outRadius + m_inRadius * 2 / 3,
                        this->height() / 2 - m_outRadius + m_inRadius * 2 / 3);
    m_point[6] = QPoint(this->width() / 2 - m_outRadius,
                        this->height() / 2 - m_outRadius + m_inRadius); 
    m_point[7] = QPoint(this->width() / 2 - m_outRadius - m_inRadius * 2 / 3,
                        this->height() / 2 - m_outRadius + m_inRadius * 2 / 3); 

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, [=]() {
        colorLoop(m_color);
        update();
    });
    this->close();
}

WaitWidget::~WaitWidget()
{
}

WaitWidget *WaitWidget::instance(QWidget *parent){
    if (!m_instance)
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!m_instance)
        {
            m_instance = new WaitWidget(parent);
        }
    }
    return m_instance;
}


void WaitWidget::startAnimation()
{
    m_displayedWhenStopped = false;
    m_timer->start(100);
    this->show();
}

void WaitWidget::stopAnimation()
{
    m_timer->stop();
    m_displayedWhenStopped = true;
    update();
    this->close();
}

void WaitWidget::colorLoop(QColor *color)
{
    QColor tmp = m_color[7];
    int tmpInt = m_outRadiuslist[7];
    for (int var = 7; var > 0; --var)
    {
        m_color[var] = m_color[var - 1];
        m_outRadiuslist[var] = m_outRadiuslist[var - 1];
    }
    m_color[0] = tmp;
    m_outRadiuslist[0] = tmpInt;
}

void WaitWidget::initStyle()
{
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_ShowModal, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setFixedSize(500, 500);
}

void WaitWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    for (int var = 0; var < 8; ++var)
    {
        QPen pen(m_color[var]); 
        pen.setWidth(1);
        pen.setStyle(Qt::CustomDashLine);
        painter.setPen(pen);
        painter.setBrush(QBrush(m_color[var], Qt::SolidPattern));
        painter.drawEllipse(m_point[var], m_outRadiuslist[var], m_outRadiuslist[var]);
    }
}
