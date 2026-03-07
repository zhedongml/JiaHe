#include "TipsDialog.h"

TipsDialog::TipsDialog(QString title, QString msg, QString yesInfo, QString noInfo, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	ui.label_title->setText(title);
	ui.label->setText(msg);
	ui.pushButton_OK->setText(yesInfo);
	ui.pushButton_Cancel->setText(noInfo);

	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(ui.label_title->sizePolicy().hasHeightForWidth());
	ui.label_title->setSizePolicy(sizePolicy);
	ui.label_title->setText(QCoreApplication::translate("TipsDialog", "<html><head/><body><p align=\"center\"><span style=\" font-size:12pt; font-weight:600;\">Warning</span></p></body></html>", nullptr));

	setWindowFlag(Qt::WindowCloseButtonHint, false);

	connect(ui.pushButton_OK, &QPushButton::clicked, this, &TipsDialog::on_pushButton_OK_clicked);
	connect(ui.pushButton_Cancel, &QPushButton::clicked, this, &TipsDialog::on_pushButton_Cancel_clicked);
}

TipsDialog::~TipsDialog()
{}

void TipsDialog::on_pushButton_OK_clicked()
{
	done(Accepted);
	this->close();
}

void TipsDialog::on_pushButton_Cancel_clicked()
{
	done(Rejected);
	this->close();
}
