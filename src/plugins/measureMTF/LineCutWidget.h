#pragma once
#include "ui_LineCutWidget.h"
#include "PrjCommon/CustomPlot.h"
#include <QAction>

class LineCutWidget : public QWidget
{
	Q_OBJECT

public:
    LineCutWidget(QWidget *parent = nullptr);
  ~LineCutWidget();

     void setAction(QAction *action);

public slots:
	void onUpdateCutLine(QList<QPointF>);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::LineCutWidgetClass ui;
    CustomPlot* plotProfileChart;
    QAction *m_action = nullptr;
};
