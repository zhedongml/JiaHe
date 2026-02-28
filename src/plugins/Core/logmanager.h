#pragma once
#include "core_global.h"
#include <QToolButton>
#include <QLabel>
#include <QObject>
#include <QTimeLine>
namespace Core {
	class OutputPaneToggleButton;
	class OutputWindow;
	class CORE_EXPORT LogManager : public QWidget
	{
		Q_OBJECT

	public:
	public:
		static LogManager* instance();
		void updateStatusButtons(bool visible);
		static void updateMaximizeButton(bool maximized);

		static int outputPaneHeightSetting();
		static void setOutputPaneHeightSetting(int value);

	public slots:
		void slotHide();
		static void toggleMaximized();

	protected:
		void focusInEvent(QFocusEvent* e) override;

	private:
		// the only class that is allowed to create and destroy
		friend class MainWindow;

		static void create();
		static void initialize();
		static void destroy();

		explicit LogManager(QWidget* parent = nullptr);
		~LogManager() override;

		void shortcutTriggered(int idx);
		void clearPage();
		void showPage(int idx);
		void buttonTriggered(int idx);

		QLabel* m_titleLabel = nullptr;
		QAction* m_clearAction = nullptr;
		QToolButton* m_clearButton = nullptr;
		QToolButton* m_closeButton = nullptr;

		QAction* m_minMaxAction = nullptr;
		QToolButton* m_minMaxButton = nullptr;

		QWidget* m_toolBar = nullptr;

		QWidget* m_buttonsWidget = nullptr;
		QIcon m_minimizeIcon;
		QIcon m_maximizeIcon;
		int m_outputPaneHeightSetting = 0;
		OutputPaneToggleButton* m_toggleButton = nullptr;
		OutputWindow* m_outputWindow = nullptr;
	};
	class BadgeLabel
	{
	public:
		BadgeLabel();
		void paint(QPainter* p, int x, int y, bool isChecked);
		void setText(const QString& text);
		QString text() const;
		QSize sizeHint() const;

	private:
		void calculateSize();

		QSize m_size;
		QString m_text;
		QFont m_font;
		static const int m_padding = 6;
	};
	class OutputPaneToggleButton : public QToolButton
	{
		Q_OBJECT
	public:
		OutputPaneToggleButton(int number, const QString& text, QAction* action,
			QWidget* parent = nullptr);
		QSize sizeHint() const override;
		void paintEvent(QPaintEvent*) override;
		void flash(int count = 3);
		void setIconBadgeNumber(int number);

	private:
		void updateToolTip();
		void checkStateSet() override;

		QString m_number;
		QString m_text;
		QAction* m_action;
		QTimeLine* m_flashTimer;
		BadgeLabel m_badgeNumberLabel;
	};

}