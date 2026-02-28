#include "reciperesultwidget.h"

RecipeResultWidget::RecipeResultWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::RecipeResultWidgetClass())
{
	ui->setupUi(this);
    ui->resultImgView->hide();
}

RecipeResultWidget::~RecipeResultWidget()
{
	// delete ui;
}
