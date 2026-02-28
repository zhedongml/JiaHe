#pragma once
#include "ui_loginDialog.h"

class LoginForm : public QDialog
{
	Q_OBJECT
public:
	LoginForm();
	~LoginForm();
  private slots:
	void on_idInput_returnPressed();
	void on_idInput_textChanged();
	void loginButton_clicked();
private:
	Ui::loginDialogClass ui;
	bool isPassed = false;
};


