#include "PolarizerWidget.h"
#include "ThorlabsMode.h"
#include <QMessageBox>
#include <QtConcurrent>

PolarizerWidget::PolarizerWidget(QString toolboxName, QWidget* parent):IToolBox(toolboxName, parent)
{
	ui.setupUi(this);

	ui.polarizerEdit->setEnabled(false);
	ui.btn_polarizerMove->setEnabled(false);
	ui.btn_refresh->setEnabled(false);
	ui.state_lab->setText("Not connected.");
	connect(&m_watcher, &QFutureWatcher<Result>::finished, this, &PolarizerWidget::handleFinished);
	connect(ui.btn_refresh, &QPushButton::clicked, this, &PolarizerWidget::handleFinished);
}

PolarizerWidget::~PolarizerWidget()
{
}

void PolarizerWidget::on_btn_connect_polarizer_clicked()
{
	Result ret = ThorlabsMode::instance()->Connect();
	if (!ret.success)
	{
		QMessageBox::critical(this, "Connect polarizer error", "Connect polarizer Fail!", QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	ui.state_lab->setText("Connected.");
	ui.polarizerEdit->setEnabled(true);
	ui.btn_polarizerMove->setEnabled(true);
	ui.btn_refresh->setEnabled(true);
}

void PolarizerWidget::on_btn_disconnect_polarizer_clicked()
{
	if (!ThorlabsMode::instance()->Disconnect())
	{
		QMessageBox::critical(this, "DisConnect polarizer error","Disconnect polarizer Fail", QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	ui.state_lab->setText("DisConnected.");
	ui.polarizerEdit->setEnabled(false);
	ui.btn_polarizerMove->setEnabled(false);
	ui.btn_refresh->setEnabled(false);
}

void PolarizerWidget::on_btn_polarizerMove_clicked()
{
	if (!ThorlabsMode::instance()->IsConnected())
	{
		QMessageBox::critical(this, "Polarizer move error", "Polarizer not connect!", QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}
	if (ui.polarizerEdit->text().isEmpty())
	{
		QMessageBox::critical(this, "Polarizer move error", "Move position is empty!", QMessageBox::Ok, QMessageBox::NoButton);
		return;
	}

	QFuture<Result> future = QtConcurrent::run([=]() {
		return ThorlabsMode::instance()->AbsMoveSync(ui.polarizerEdit->text().toDouble());
		});
	m_watcher.setFuture(future);
}

void PolarizerWidget::handleFinished()
{
	double pos = ThorlabsMode::instance()->GetPosition() / 1000.0;
	ui.lable_curpos->setText(QString::number(pos, 'f', 3));
}
