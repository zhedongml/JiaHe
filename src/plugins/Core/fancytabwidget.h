#pragma once

#include <QWidget>
#include "fancytabbar.h"
#include <QStatusBar>
#include <QStackedLayout>
#include  <QStackedWidget>
QT_BEGIN_NAMESPACE
namespace Ui { class FancyTabWidgetClass; };
class QSplitter;
QT_END_NAMESPACE
namespace Core {
    class FancyTabWidget : public QWidget
    {
        Q_OBJECT

    public:
        FancyTabWidget(QWidget* parent = nullptr);
        ~FancyTabWidget();
        void insertTab(int index, QWidget* tab, const QIcon& icon, const QString& label, bool hasMenu);
        void insertPartialUpdateTab(int index, QWidget* logicTab, QWidget* resultTab, const QIcon& icon, const QString& label, bool hasMenu);
        void removeTab(int index);
        void setBackgroundBrush(const QBrush& brush);
        void addCornerWidget(QWidget* widget);
        void insertCornerWidget(int pos, QWidget* widget);
        int cornerWidgetCount() const;
        void setTabToolTip(int index, const QString& toolTip);

        void paintEvent(QPaintEvent* event) override;
        int currentIndex() const;
        QStatusBar* statusBar() const;

        void setTabEnabled(int index, bool enable);
        bool isTabEnabled(int index) const;

        void setIconsOnly(bool iconsOnly);

        bool isSelectionWidgetVisible() const;

        void addStackWidget(QWidget* widget, int index);

        void addLogViewer(QWidget* widget);
        QSplitter* getSplitter();
    signals:
        void currentAboutToShow(int index);
        void currentChanged(int index);
        void menuTriggered(int index, QMouseEvent* event);
        void topAreaClicked(Qt::MouseButton button, Qt::KeyboardModifiers modifiers);

    public slots:
        void setCurrentIndex(int index);
        void setSelectionWidgetVisible(bool visible);

    private:
        void showWidget(int index);

        FancyTabBar* m_tabBar;
        QWidget* m_cornerWidgetContainer;
        QStackedWidget* m_modesStack;
        QWidget* m_selectionWidget;
        QStatusBar* m_statusBar;
        QSplitter* splitter = nullptr;
    private:
        Ui::FancyTabWidgetClass* ui;
    };
}