#include "loginform.h"
//#include "logindata.h"

LoginForm::LoginForm()
{
	ui.setupUi(this);
	setWindowIcon(QIcon(":/image/images/mlico.ico"));
	this->setFixedSize(this->width(), this->height()-5);
	ui.loginBtn->setEnabled(false);
    ui.label_2->hide();
    ui.label_4->hide();
    ui.userlevel->hide();
    ui.validation->clear();
	connect(ui.loginBtn, SIGNAL(clicked()), this, SLOT(loginButton_clicked()));

	this->setStyleSheet("background-color:rgb(52, 115, 157);");
}

LoginForm::~LoginForm()
{
}
void LoginForm::on_idInput_returnPressed()
{
    loginButton_clicked();
}
void LoginForm::on_idInput_textChanged()
{
	if ((ui.idInput->text()) == "")
	{
		ui.userlevel->clear();
        ui.validation->clear();
		ui.loginBtn->setEnabled(false);
    }
    else
        ui.loginBtn->setEnabled(true);
}
void LoginForm::loginButton_clicked()
{
    QString userName = ui.idInput->text().trimmed();
    if (userName == "1")
	{
		this->setResult(QDialog::Accepted);
		this->hide();
	}
	else
	{
		this->setResult(QDialog::Rejected);
        ui.validation->show();
        ui.validation->setText("User ID error,please check!");
        ui.validation->setStyleSheet("color:red;");
        ui.validation->setWordWrap(true);
		//this->hide();
	}
}