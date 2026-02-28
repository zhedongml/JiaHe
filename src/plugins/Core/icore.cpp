#include "icore.h"
#include "mainwindow.h"
namespace Core {
    static ICore* m_instance = nullptr;
    static MainWindow* m_mainwindow = nullptr;

    /*!
        Returns the pointer to the instance. Only use for connecting to signals.
    */
    ICore* ICore::instance()
    {
        return m_instance;
    }

    /*!
        \internal
    */
    ICore::ICore(MainWindow* mainwindow)
    {
        m_instance = this;
        m_mainwindow = mainwindow;
    }

    /*!
        \internal
    */
    ICore::~ICore()
    {
        m_instance = nullptr;
        m_mainwindow = nullptr;
    }

    QMainWindow* ICore::mainWindow()
    {
        return m_mainwindow;
    }

    QStatusBar* ICore::statusBar()
    {
        return m_mainwindow->statusBar();
    }

    void ICore::addContextObject(IContext* context)
    {
        m_mainwindow->addContextObject(context);
    }

    void ICore::removeContextObject(IContext* context)
    {
        m_mainwindow->removeContextObject(context);
    }

    IContext* ICore::contextObject(QWidget* widget)
    {
        return m_mainwindow->contextObject(widget);
    }
    void ICore::updateAdditionalContexts(const Context& remove, const Context& add, ContextPriority priority)
    {
        m_mainwindow->updateAdditionalContexts(remove, add, priority);
    }

    void ICore::addAdditionalContext(const Context& context, ContextPriority priority)
    {
        m_mainwindow->updateAdditionalContexts(Context(), context, priority);
    }

    void ICore::raiseWindow(QWidget* widget)
    {
        if (!widget)
            return;
        QWidget* window = widget->window();
        if (window && window == m_mainwindow)
        {
            m_mainwindow->raiseWindow();
        }
        else
        {
            window->raise();
            window->activateWindow();
        }
    }

    void ICore::restart()
    {
        m_mainwindow->restart();
    }

}