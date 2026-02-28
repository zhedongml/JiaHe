#pragma once

#include <QWidget>
#include "ui_reciperesultwidget.h"

class RecipeResultWidget : public QWidget
{
	Q_OBJECT

public:
	RecipeResultWidget(QWidget *parent = nullptr);
	~RecipeResultWidget();

private:
	Ui::RecipeResultWidgetClass *ui;
};
