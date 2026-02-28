#include "SelectFiducialWidget.h"
#include <QGridLayout>
#include <QHboxLayout>
#include "fiducialpoint.h"
#include <QDebug>

SelectFiducialWidget::SelectFiducialWidget(QWidget* parent)
{
	this->resize(800, 600);
	this->setWindowFlag(Qt::WindowCloseButtonHint, false);
	this->setWindowTitle(tr("Select Fiducial Point"));

	m_pBtnAccept = new QPushButton(tr("Confirm"), this);
	m_pBtnCancle = new QPushButton(tr("Cancel"), this);
	QHBoxLayout* hlay = new QHBoxLayout();
	hlay->addWidget(m_pBtnAccept);
	hlay->addSpacing(5);
	hlay->addWidget(m_pBtnCancle);
	m_pBtnAccept->setMinimumHeight(40);

	m_pViewImage = new CameraViewer(this);

	QGridLayout* grid = new QGridLayout();
	grid->addWidget(m_pViewImage, 0, 0);
	grid->addLayout(hlay, 1, 0);
	grid->setRowStretch(0, 1);
	setLayout(grid);

	connect(m_pBtnAccept, SIGNAL(clicked(bool)), this, SLOT(onSlotBtnAcceptClicked()));
	connect(m_pBtnCancle, SIGNAL(clicked(bool)), this, SLOT(onSlotBtnCancleClicked()));
}

SelectFiducialWidget::~SelectFiducialWidget()
{
}

void SelectFiducialWidget::setFiducialImage(QImage& image)
{
	if (nullptr == m_pViewImage)
	{
		return;
	}

	QList<QGraphicsItem*> items = m_pViewImage->items();
	for (QGraphicsItem* item : items)
	{
		FiducialPoint* fid = dynamic_cast<FiducialPoint*>(item);
		if (fid == NULL)
		{
			continue;
		}

		fid->disconnect();
		delete fid;
		fid = nullptr;
	}

	m_pViewImage->Show(image);

	m_pViewImage->EnableCross();

	QGraphicsScene* sceneItem = m_pViewImage->scene();
	if (sceneItem != nullptr)
	{
		QPointF scenePos = m_pViewImage->mapToScene(QPoint(image.width() / 2, image.height() / 2));
		FiducialPoint* fiducial = new FiducialPoint(NULL, QPointF(scenePos.x(), scenePos.y()));
		fiducial->setZValue(1);
		fiducial->setId(1);
		sceneItem->addItem(fiducial);
		connect(fiducial, SIGNAL(postionChanged(qreal, qreal, int)), this, SLOT(onSlotFidPosChanged(qreal, qreal, int)));
		m_pViewImage->scene()->update();
		m_FiducialPoint = scenePos;
	}
}

QPointF SelectFiducialWidget::getFiducialPos()
{
	return m_FiducialPoint;
}

void SelectFiducialWidget::onSlotBtnAcceptClicked()
{
	done(1);
}

void SelectFiducialWidget::onSlotBtnCancleClicked()
{
	done(-1);
}

void SelectFiducialWidget::onSlotFidPosChanged(qreal x, qreal y, int id)
{
	m_FiducialPoint = QPointF(x, y);
}