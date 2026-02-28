#include "DecoderWidget.h"
#include <QGraphicsScene>
#include <QMessagebox>

DecoderWidget::DecoderWidget(QString toolboxName, QWidget *parent)
	: IToolBox(toolboxName, parent)
{
	ui.setupUi(this);

	ui.lineEdit_barcode->setEnabled(false);
	ui.status->setText("Not connected.");

	m_model = &DecoderModel::instance();
	connect(m_model, SIGNAL(connectStatus(bool, QString)), this, SLOT(connectStatus(bool, QString)));

	ui.btn_captureImage->hide();
	ui.graphicsView->hide();
}

DecoderWidget::~DecoderWidget()
{}

void DecoderWidget::on_btn_connect_clicked()
{
	m_model->connect();
}

void DecoderWidget::on_btn_disconnect_clicked()
{
	m_model->disconnect();
}

void DecoderWidget::on_btn_read_clicked()
{
	QString msg;
	Result ret = m_model->queryBarcode(msg);
	if (!ret.success)
	{
		QMessageBox::critical(this, "Read barcode error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}

	ui.lineEdit_barcode->setText(msg);
}

void DecoderWidget::on_btn_captureImage_clicked()
{
	QString imagePath;
	Result ret = m_model->captureImage(imagePath);
	if (!ret.success)
	{
		QMessageBox::critical(this, "Capture image error", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}

	if(m_item == nullptr){
		QGraphicsScene* scene = new QGraphicsScene();
		ui.graphicsView->setScene(scene);
		m_item = new QGraphicsPixmapItem();
		scene->addItem(m_item);
	}

	//imagePath = "d:/projector.png";
	QPixmap pixmap(imagePath);
	m_item->setPixmap(pixmap);
	ui.graphicsView->scene()->setSceneRect(m_item->boundingRect());
	ui.graphicsView->fitInView(ui.graphicsView->sceneRect(), Qt::KeepAspectRatio);

}

void DecoderWidget::connectStatus(bool status, QString msg)
{
	ui.status->setText(msg);
}

