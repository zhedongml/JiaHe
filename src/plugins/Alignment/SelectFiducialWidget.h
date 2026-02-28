#pragma once

#include <QDialog>
#include <QPushButton>
#include "CameraModel.h"
#include "CameraViewer.h"
#include "opencv2/opencv.hpp"

class SelectFiducialWidget : public QDialog
{
	Q_OBJECT
public:
	explicit SelectFiducialWidget(QWidget* parent = nullptr);
	~SelectFiducialWidget();

	void setFiducialImage(QImage& image);

	QPointF getFiducialPos();

public slots:
	void onSlotBtnAcceptClicked();
	void onSlotBtnCancleClicked();

	void onSlotFidPosChanged(qreal x, qreal y, int id);

private:
	CameraViewer* m_pViewImage{ nullptr };
	QPushButton* m_pBtnAccept{ nullptr };
	QPushButton* m_pBtnCancle{ nullptr };
	QPointF m_FiducialPoint = QPointF(0, 0);
};

