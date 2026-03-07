#pragma once
#include <QMainWindow>
#include "icontext.h"
#include <unordered_map>
#include "icore.h"

#include "DockManager.h"
#include "DockWidget.h"
#include <QTimer>

class QToolButton;

namespace Core {
	class FancyTabWidget;
	class RightPanePlaceHolder;
	class RightPaneWidget;
	class ICore;
	class ModeManager;
	enum class Side {
		Left,
		Right
	};
	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		MainWindow(QObject* parent=nullptr);
		~MainWindow();
		void init();
		void extensionsInitialized();
		void aboutToShutdown();
		void addContextObject(IContext* context);
		void removeContextObject(IContext* context);
		IContext* contextObject(QWidget* widget) const;

		void updateAdditionalContexts(const Context& remove, const Context& add, ICore::ContextPriority priority);

		void addToolBox(QWidget* wdg, QString title = "Tab");
		void addMenuAction(QAction* action, QString id, QString parentId);
		void addMenuAction(QAction* action, QString id, QString parentId, QString groupId);

		void restart();
		void addPreCloseListener(const std::function<bool()>& listener);

	private:
		void initRightDock();
		bool isAllFloating();
		void initStopBtn();

	protected:
		void closeEvent(QCloseEvent* event) override;

	private slots:
		void updateFocusWidget(QWidget* old, QWidget* now);

		// Dock
		void setCurrent(QList<ads::CDockWidget*> docks);
		void watchAndHandleRight();

	public slots:
		void raiseWindow();
		void exit();

	signals:
		void finishDock(QList<ads::CDockWidget*>);

	private:
		FancyTabWidget* m_modeStack = nullptr;
		ModeManager* m_modeManager = nullptr;
		ICore* m_coreImpl = nullptr;
		RightPanePlaceHolder* panePlaceHolder;
		std::unordered_map<QWidget*, IContext*> m_contextWidgets;
		QToolButton* m_toggleRightSideBarButton = nullptr;
		RightPaneWidget* m_rightPaneWidget = nullptr;
		Context m_highPrioAdditionalContexts;
		Context m_lowPrioAdditionalContexts;
		QList<IContext*> m_activeContext;
		void updateContext();
		void registerDefaultContainers();
		void registerDefaultActions();
		void updateContextObject(const QList<IContext*>& context);
		void setSidebarVisible(bool visible, Side side);

	private:
		ads::CDockAreaWidget* dockPageManager;
		int m_Toolbox = 0;
		bool floatingFlag = false;
		QTimer* timer = new QTimer(this);
		QList<ads::CDockWidget*> m_docks;
		ads::CDockManager* m_dockManager = nullptr;

		bool m_askConfirmationBeforeExit = false;
		QList<std::function<bool()>> m_preCloseListeners;
	};
}