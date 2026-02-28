#pragma once

#include <QWidget>
#include "ui_mtfsettingwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MTFSettingWidgetClass; };
QT_END_NAMESPACE

class MTFSettingWidget : public QWidget
{
	Q_OBJECT

public:
	MTFSettingWidget(QWidget *parent = nullptr);
	~MTFSettingWidget();

private:
	Ui::MTFSettingWidgetClass *ui;
};
