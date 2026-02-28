#pragma once

#include <QWidget>
#include <QTimer>
#include "prjcommon_global.h"

class PRJCOMMON_EXPORT WaitWidget : public QWidget
{
    Q_OBJECT

  public:
    static WaitWidget *instance(QWidget *parent = nullptr);
    ~WaitWidget();
    void startAnimation();
    void stopAnimation();
  private:
    explicit WaitWidget(QWidget *parent = nullptr);
    void colorLoop(QColor *);
    void initStyle();

  protected:
    void paintEvent(QPaintEvent *event);

  private:
    QColor m_color[8];
    QPoint m_point[8];
    int m_inRadius;
    int m_outRadius;
    int m_outRadiuslist[8];
    QTimer *m_timer;
    bool m_displayedWhenStopped = true;

    static WaitWidget *m_instance;
};
