#pragma once
#include "core_global.h"
#include <QObject>
#include <QMainWindow>
#include "icontext.h"
namespace Core {
	class Context;
	class MainWindow;
	class CORE_EXPORT ICore : public QObject
	{
		Q_OBJECT
		friend class MainWindow;
	public:
		explicit ICore(MainWindow* mw);
		~ICore() override;
		enum class ContextPriority {
			High,
			Low
		};

		static ICore* instance();
		static QMainWindow* mainWindow();
		static QStatusBar* statusBar();
		static void addContextObject(IContext* context);
		static void removeContextObject(IContext* context);
		static IContext* contextObject(QWidget* widget);
		static void updateAdditionalContexts(const Context& remove, const Context& add,
			ContextPriority priority = ContextPriority::Low);
		static void addAdditionalContext(const Context& context,
			ContextPriority priority = ContextPriority::Low);
		static void raiseWindow(QWidget* widget);

		static void restart();
	signals:
		void coreAboutToClose();
		void contextChanged(const Core::Context& context);
		void contextAboutToChange(const QList<Core::IContext*>& context);
		void emgStopBtnClicked();
	};
}
