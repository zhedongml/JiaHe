#pragma once

#include <QWidget>
#include <QMutex>
#include <queue>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsItemGroup>
#include "ui_CollimatorWidget.h"
#include "ml_camera.h"

//Collimated light tube cross display interface
class CollimatorWidget : public QWidget
{
	Q_OBJECT

public:
	CollimatorWidget(QWidget* parent = nullptr);
	~CollimatorWidget();

public:
	void ZoomInit();
	void ShowCenterCross();

signals:
	void showImageSignal();

private slots:
	void ShowImage();
private:
	Ui::CollimatorWidgetClass ui;

	QImage m_currentImage;
	QPixmap m_currentPixmap;
	QGraphicsPixmapItem* m_PixmapItem;
	QGraphicsScene* m_Scene;
	QGraphicsItemGroup* m_showCenterCrossGroup = nullptr; //Image assisted crosshair
};
