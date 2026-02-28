#include "CollimatorWidget.h"
#include "CollimatedLightTubeMode.h"
#define IMAGEMAXNUM 3

CollimatorWidget::CollimatorWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	m_Scene = new QGraphicsScene();
	m_PixmapItem = new QGraphicsPixmapItem();
	m_Scene->addItem(m_PixmapItem);
	m_Scene->setBackgroundBrush(QColor("#f8ffff"));
	ui.CollimatorShow->setScene(m_Scene);

	setWindowTitle("Collimated light tube crosshair display");
	setWindowFlags(Qt::Window);
	setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
	connect(CollimatedLightTubeMode::GetInstance(), SIGNAL(showImageSignal()), this, SLOT(ShowImage()));
}

CollimatorWidget::~CollimatorWidget()
{
	delete m_PixmapItem;
	delete m_Scene;
	/*if (m_showCenterCrossGroup)
		delete m_showCenterCrossGroup;*/
}

void CollimatorWidget::ZoomInit()
{
	ui.CollimatorShow->fitInView(m_PixmapItem, Qt::KeepAspectRatio);
}

void CollimatorWidget::ShowCenterCross()
{
	if (m_showCenterCrossGroup != NULL)
	{
		QList<QGraphicsItem*> items = m_showCenterCrossGroup->childItems();

		for (QGraphicsItem* item : items) {
			m_showCenterCrossGroup->removeFromGroup(item);
			delete item;
		}
		m_Scene->removeItem(m_showCenterCrossGroup);
	}

	if (!m_showCenterCrossGroup)
		m_showCenterCrossGroup = new QGraphicsItemGroup();
	qreal height = m_Scene->height();
	qreal width = m_Scene->width();
	QPen pen(Qt::red);
	pen.setWidth(1);
	QGraphicsLineItem* line1 = new QGraphicsLineItem(QLineF(0, (double)height / 2, width, (double)height / 2));
	line1->setPen(pen);
	QGraphicsLineItem* line2 = new QGraphicsLineItem(QLineF((double)width / 2, 0, (double)width / 2, height));
	line2->setPen(pen);
	m_showCenterCrossGroup->addToGroup(line1);
	m_showCenterCrossGroup->addToGroup(line2);
	m_Scene->addItem(m_showCenterCrossGroup);
}

void CollimatorWidget::ShowImage()
{
	m_currentImage = CollimatedLightTubeMode::GetInstance()->GetQImage();
	m_currentPixmap = QPixmap::fromImage(m_currentImage);
	if (!m_currentPixmap.isNull())
	{
		m_PixmapItem->setPixmap(m_currentPixmap);

		m_Scene->setSceneRect(0, 0, m_currentPixmap.width(), m_currentPixmap.height());

		ui.CollimatorShow->viewport()->update();
		ui.CollimatorShow->show();
	}
}