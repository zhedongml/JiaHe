#include "fancytabwidget.h"
#include "ui_fancytabwidget.h"
#include <QPainter>
#include <QStyleFactory>
#include <QStyleOption>
#include "logmanager.h"
#include <QSplitter>
#include <QHBoxLayout>
#include "theme/theme_p.h"
#include "theme/stylehelper.h"
#include <QMouseEvent>
using namespace Core;
static int clamp(float x)
{
	const int val = x > 255 ? 255 : static_cast<int>(x);
	return val < 0 ? 0 : val;
}
FancyTabWidget::FancyTabWidget(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::FancyTabWidgetClass())
{
	ui->setupUi(this);
	QWidget* p = ui->tabholder;
	p->layout()->setSpacing(0);
	p->layout()->setContentsMargins(0, 0, 0, 0);
	m_tabBar = new FancyTabBar(p);
	m_tabBar->setObjectName("ModeSelector");
	m_selectionWidget = ui->selectionWidget;
	auto selectionLayout = (QVBoxLayout*)m_selectionWidget->layout();
	selectionLayout->setSpacing(0);
	selectionLayout->setContentsMargins(0, 20, 0, 0);
	selectionLayout->addWidget(m_tabBar);
	selectionLayout->addStretch(1);
	m_selectionWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	m_cornerWidgetContainer = new QWidget(this);
	m_cornerWidgetContainer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	m_cornerWidgetContainer->setAutoFillBackground(false);
	auto cornerWidgetLayout = new QVBoxLayout;
	cornerWidgetLayout->setSpacing(0);
	cornerWidgetLayout->setContentsMargins(0, 0, 0, 0);
	cornerWidgetLayout->addStretch();
	m_cornerWidgetContainer->setLayout(cornerWidgetLayout);

	selectionLayout->addWidget(m_cornerWidgetContainer, 0);
	m_modesStack = ui->commonstackedWidget;
	//m_tabBar->insertTab(0,QIcon(":/img/information.svg"), "Information",false,STACKPOSTION::FLLPAGE,0);
	//m_tabBar->insertTab(0,QIcon(":/img/network.svg"), "Network",false, STACKPOSTION::FLLPAGE,1);
	//m_tabBar->insertTab(0,QIcon(":/img/settings.svg"), "Settings",false, STACKPOSTION::LOGICANDRESULTBOTH,0);

	splitter = new QSplitter(Qt::Vertical, this);
	QHBoxLayout* hLayout = (QHBoxLayout*)this->layout();
	hLayout->insertWidget(1, splitter, 7);
	//QSplitter* splitter = new QSplitter(Qt::Vertical, this);
	splitter->insertWidget(0, ui->commonstackedWidget);
	splitter->setStretchFactor(0, 2);
	splitter->setCollapsible(0, false);
	connect(m_tabBar, &FancyTabBar::currentAboutToChange, this, &FancyTabWidget::currentAboutToShow);
	connect(m_tabBar, &FancyTabBar::currentChanged, this, &FancyTabWidget::showWidget);
	connect(m_tabBar, &FancyTabBar::menuTriggered, this, &FancyTabWidget::menuTriggered);
}
void FancyTabWidget::addLogViewer(QWidget* widget)
{
	if (splitter)
	{
		splitter->insertWidget(1, widget);
		splitter->setStretchFactor(1, 0);
		splitter->setCollapsible(1, false);
	}
}
QSplitter* FancyTabWidget::getSplitter()
{
	return splitter;
}
FancyTabWidget::~FancyTabWidget()
{
	delete ui;
}

void FancyTabWidget::setSelectionWidgetVisible(bool visible)
{
	m_selectionWidget->setVisible(visible);
}

bool FancyTabWidget::isSelectionWidgetVisible() const
{
	return m_selectionWidget->isVisible();
}

void FancyTabWidget::addStackWidget(QWidget* widget, int index)
{
	m_modesStack->insertWidget(index, widget);
}

void FancyTabWidget::insertTab(int index, QWidget* tab, const QIcon& icon, const QString& label, bool hasMenu)
{
	m_modesStack->insertWidget(m_modesStack->count(), tab);
	m_tabBar->insertTab(index, icon, label, hasMenu,STACKPOSTION::FULLPAGE, m_modesStack->count()-1);
}
void FancyTabWidget::insertPartialUpdateTab(int index, QWidget* logicTab, QWidget* resultTab, const QIcon& icon, const QString& label, bool hasMenu)
{
	if (logicTab && resultTab) {
		ui->logicstack->insertWidget(ui->logicstack->count(), logicTab);
		ui->resultstack->insertWidget(ui->resultstack->count(), resultTab);
		m_tabBar->insertTab(index, icon, label, hasMenu, STACKPOSTION::LOGICANDRESULTBOTH, ui->logicstack->count() - 1);
	}
	else {
		if (logicTab) {
			ui->logicstack->insertWidget(ui->logicstack->count(), logicTab);
			m_tabBar->insertTab(index, icon, label, hasMenu, STACKPOSTION::LOGICPAGE, ui->logicstack->count() - 1);
		}
		else if (resultTab) {
			ui->resultstack->insertWidget(ui->resultstack->count(), resultTab);
			m_tabBar->insertTab(index, icon, label, hasMenu, STACKPOSTION::RESULTPAGE, ui->resultstack->count() - 1);
		}
	}
}
void FancyTabWidget::removeTab(int index)
{
	FancyTab* tabPtr = m_tabBar->getTab(index);
	if (tabPtr->fullpageStackId >= 0) {
		m_modesStack->removeWidget(m_modesStack->widget(tabPtr->fullpageStackId));
	}
	else
	{
		if (tabPtr->resultPartStackId >= 0) {
			ui->resultstack->removeWidget(ui->resultstack->widget(tabPtr->resultPartStackId));
		}
		if (tabPtr->logicPartStackId >= 0) {
			ui->logicstack->removeWidget(ui->logicstack->widget(tabPtr->logicPartStackId));
		}
	}
	m_tabBar->removeTab(index);
}

void FancyTabWidget::setBackgroundBrush(const QBrush& brush)
{
	QPalette pal;
	pal.setBrush(QPalette::Mid, brush);
	m_tabBar->setPalette(pal);
	m_cornerWidgetContainer->setPalette(pal);
}

void FancyTabWidget::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)
	if (m_selectionWidget->isVisible()) {
		QPainter painter(this);

		QRect rect = m_selectionWidget->rect().adjusted(0, 0, 1, 0);
		rect = QStyle::visualRect(layoutDirection(), geometry(), rect);
		const QRectF boderRect = QRectF(rect).adjusted(0.5, 0.5, -0.5, -0.5);

		if (creatorTheme()->flag(Theme::FlatToolBars)) {
			painter.fillRect(rect, StyleHelper::baseColor());
			painter.setPen(StyleHelper::toolBarBorderColor());
			painter.drawLine(boderRect.topRight(), boderRect.bottomRight());
		}
		else {
			StyleHelper::verticalGradient(&painter, rect, rect);
			painter.setPen(StyleHelper::borderColor());
			painter.drawLine(boderRect.topRight(), boderRect.bottomRight());

			const QColor light = StyleHelper::sidebarHighlight();
			painter.setPen(light);
			painter.drawLine(boderRect.bottomLeft(), boderRect.bottomRight());
		}
	}
}

void FancyTabWidget::insertCornerWidget(int pos, QWidget* widget)
{
	auto layout = static_cast<QVBoxLayout*>(m_cornerWidgetContainer->layout());
	layout->insertWidget(pos, widget);
}

int FancyTabWidget::cornerWidgetCount() const
{
	return m_cornerWidgetContainer->layout()->count();
}

void Core::FancyTabWidget::setTabToolTip(int index, const QString& toolTip)
{
	m_tabBar->setTabToolTip(index, toolTip);
}

void FancyTabWidget::addCornerWidget(QWidget* widget)
{
	m_cornerWidgetContainer->layout()->addWidget(widget);
}

int FancyTabWidget::currentIndex() const
{
	return m_tabBar->currentIndex();
}

QStatusBar* FancyTabWidget::statusBar() const
{
	return m_statusBar;
}

void FancyTabWidget::setCurrentIndex(int index)
{
	m_tabBar->setCurrentIndex(index);
}

void FancyTabWidget::showWidget(int index)
{
	//m_modesStack->setCurrentIndex(index);
	//QWidget* w = m_modesStack->currentWidget();
	//if (QWidget* focusWidget = w->focusWidget())
	//	w = focusWidget;
	//w->setFocus();

	FancyTab* tabPtr = m_tabBar->getTab(index);
	if (tabPtr->fullpageStackId >= 0) {
		m_modesStack->setCurrentIndex(tabPtr->fullpageStackId);
		QWidget* w = m_modesStack->currentWidget();
		if (QWidget* focusWidget = w->focusWidget())
			w = focusWidget;
		w->setFocus();
	}
	else
	{
		if (tabPtr->resultPartStackId >= 0) {
			m_modesStack->setCurrentIndex(0);
			ui->resultstack->setCurrentIndex(tabPtr->resultPartStackId);
			QWidget* w = ui->resultstack->currentWidget();
			if (QWidget* focusWidget = w->focusWidget())
				w = focusWidget;
			w->setFocus();
		}
		if (tabPtr->logicPartStackId >= 0) {
			m_modesStack->setCurrentIndex(0);
			ui->logicstack->setCurrentIndex(tabPtr->logicPartStackId);
			QWidget* w = ui->logicstack->currentWidget();
			if (QWidget* focusWidget = w->focusWidget())
				w = focusWidget;
			w->setFocus();
		}
	}
	emit currentChanged(index);
}
void FancyTabWidget::setTabEnabled(int index, bool enable)
{
	m_tabBar->setTabEnabled(index, enable);
}

bool FancyTabWidget::isTabEnabled(int index) const
{
	return m_tabBar->isTabEnabled(index);
}

void FancyTabWidget::setIconsOnly(bool iconsOnly)
{
	m_tabBar->setIconsOnly(iconsOnly);
}