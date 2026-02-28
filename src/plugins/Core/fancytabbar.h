#ifndef FANCYTABBAR_H
#define FANCYTABBAR_H

#include <QWidget>
#include <QVector>
#include <QRect>

#include "fancytab.h"

namespace Core {
	enum STACKPOSTION
	{
		FULLPAGE=0,
		LOGICPAGE =1 ,
		RESULTPAGE =2,
		LOGICANDRESULTBOTH = 3
	};
	class FancyTabBar : public QWidget
	{
		Q_OBJECT

	public:
		FancyTabBar(QWidget* parent = nullptr);

		bool event(QEvent* event) override;

		void paintEvent(QPaintEvent* event) override;
		void paintTab(QPainter* painter, int tabIndex) const;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void enterEvent(QEvent* event) override;
		void leaveEvent(QEvent* event) override;
		bool validIndex(int index) const { return index >= 0 && index < m_tabs.count(); }

		QSize sizeHint() const override;
		QSize minimumSizeHint() const override;

		void setTabEnabled(int index, bool enable);
		bool isTabEnabled(int index) const;

		void insertTab(int index, const QIcon& icon, const QString& label, bool hasMenu, STACKPOSTION type, int id);
		void setEnabled(int index, bool enabled);
		FancyTab* getTab(int index);
		void removeTab(int index);
		void setCurrentIndex(int index);
		int currentIndex() const { return m_currentIndex; }

		void setTabToolTip(int index, const QString& toolTip) { m_tabs[index]->toolTip = toolTip; }
		QString tabToolTip(int index) const { return m_tabs.at(index)->toolTip; }

		void setIconsOnly(bool iconOnly);

		int count() const { return m_tabs.count(); }
		QRect tabRect(int index) const;

	signals:
		void currentAboutToChange(int index);
		void currentChanged(int index);
		void menuTriggered(int index, QMouseEvent* event);

	private:
		QRect m_hoverRect;
		int m_hoverIndex = -1;
		int m_currentIndex = -1;
		bool m_iconsOnly = false;
		QList<FancyTab*> m_tabs;
		QSize tabSizeHint(bool minimum = false) const;
	};
}
#endif // FANCYTABBAR_H