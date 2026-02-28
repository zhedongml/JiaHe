#include "statusbarmanager.h"

#include "imode.h"
#include "mainwindow.h"
#include "qsplitter.h"
#include "modemanager.h"
#include "icore.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QSplitter>
#include <QStatusBar>

namespace Core {

const char kSettingsGroup[] = "StatusBar";
const char kLeftSplitWidthKey[] = "LeftSplitWidth";

static QPointer<QSplitter> m_splitter;
static QList<QPointer<QWidget>> m_statusBarWidgets;
static QList<QPointer<IContext>> m_contexts;

/*
    Context that always returns the context of the active's mode widget (if available).
*/
class StatusBarContext : public IContext
{
public:
    StatusBarContext(QObject *parent);

    Context context() const final;
};

static QWidget *createWidget(QWidget *parent)
{
    QWidget *w = new QWidget(parent);
    w->setLayout(new QHBoxLayout);
    w->setVisible(true);
    w->layout()->setContentsMargins(0, 0, 0, 0);
    return w;
}

static void createStatusBarManager()
{
    QStatusBar *bar = ICore::statusBar();

    m_splitter = new QSplitter(bar);
    bar->insertPermanentWidget(0, m_splitter, 10);
    m_splitter->setChildrenCollapsible(false);
    // first
    QWidget *w = createWidget(m_splitter);
    w->setFixedWidth(80);
    //TODO: May not used
    //w->layout()->setContentsMargins(0, 0, 3, 0);
    m_splitter->addWidget(w);
    m_statusBarWidgets.append(w);

    QWidget *w2 = createWidget(m_splitter);
    w2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
    m_splitter->addWidget(w2);
    // second
    w = createWidget(w2);
    w2->layout()->addWidget(w);
    m_statusBarWidgets.append(w);
    // third
    w = createWidget(w2);
    w2->layout()->addWidget(w);

    m_statusBarWidgets.append(w);

    static_cast<QBoxLayout *>(w2->layout())->addStretch(1);

    QWidget *rightCornerWidget = createWidget(bar);
    bar->insertPermanentWidget(1, rightCornerWidget);
    m_statusBarWidgets.append(rightCornerWidget);

    auto statusContext = new StatusBarContext(bar);
    statusContext->setWidget(bar);
    ICore::addContextObject(statusContext);


    QObject::connect(ICore::instance(), &ICore::coreAboutToClose, [statusContext] {
        delete statusContext;
        // This is the catch-all on rampdown. Individual items may
        // have been removed earlier by destroyStatusBarWidget().
        for (const QPointer<IContext> &context : qAsConst(m_contexts)) {
            ICore::removeContextObject(context);
            delete context;
        }
        m_contexts.clear();
    });
}

void StatusBarManager::addStatusBarWidget(QWidget *widget,
                                          StatusBarPosition position,
                                          const Context &ctx)
{
    if (!m_splitter)
        createStatusBarManager();

    m_statusBarWidgets.at(position)->layout()->addWidget(widget);

    auto context = new IContext;
    context->setWidget(widget);
    context->setContext(ctx);
    m_contexts.append(context);

    ICore::addContextObject(context);
}

void StatusBarManager::destroyStatusBarWidget(QWidget *widget)
{
    const auto it = std::find_if(m_contexts.begin(), m_contexts.end(),
            [widget](const auto &context) { return context->widget() == widget; });
    if (it != m_contexts.end()) {
        delete *it;
        m_contexts.erase(it);
    }
    widget->setParent(nullptr);
    delete widget;
}

StatusBarContext::StatusBarContext(QObject *parent)
    : IContext(parent)
{
}

Context StatusBarContext::context() const
{
    IMode *currentMode = ModeManager::currentMode();
    QWidget *modeWidget = currentMode ? currentMode->widget() : nullptr;
    if (modeWidget) {
        if (IContext *context = ICore::contextObject(modeWidget))
            return context->context();
    }
    return Context();
}

} // Core
