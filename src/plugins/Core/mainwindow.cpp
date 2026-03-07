#include "mainwindow.h"
#include "icore.h"
#include "fancytabwidget.h"
#include  <qlabel.h>
#include <QApplication>
#include <qmenubar.h>
#include "fancytabwidget.h"
#include "qsplitter.h"
#include "rightpane.h"
#include "statusbarmanager.h"
#include  "qtoolbutton.h"
#include "coreconstants.h"
#include "actionmanager/actionmanager.h"
#include "actionmanager/actioncontainer.h"
#include "proxyaction.h"
#include <QDebug>
#include "modemanager.h"
#include "outputpane.h"
#include "logmanager.h"
#include "theme/manhattanstyle.h"
#include "theme/themechooser.h"
#include "theme/theme.h"
#include "theme/theme_p.h"
#include <QMessageBox>

using namespace Core;
MainWindow::MainWindow(QObject *parent)
	: QMainWindow()
{
    m_toggleRightSideBarButton = new QToolButton;
    m_modeStack = new FancyTabWidget(this);
    m_coreImpl = new ICore(this);
    //QApplication::setStyle(new DarkStyleTheme);

    if (ThemeEntry::availableThemes().isEmpty()) {
        return;
    }
    Theme::initialPalette(); // Initialize palette before setting it
    Theme* themeFromArg = ThemeEntry::createTheme("flat");
    setCreatorTheme(themeFromArg ? themeFromArg
        : ThemeEntry::createTheme(ThemeEntry::themeSetting()));
    QApplication::setStyle(new ManhattanStyle("windowsvista"));
    m_rightPaneWidget = new RightPaneWidget();

    auto paneSplitter = new QSplitter;
    paneSplitter->insertWidget(0, m_modeStack);
    panePlaceHolder = new RightPanePlaceHolder(Constants::MAIN_WINDOW);
    paneSplitter->insertWidget(1, panePlaceHolder);
    paneSplitter->setStretchFactor(0, 1);
    paneSplitter->setStretchFactor(1, 1);
    paneSplitter->setCollapsible(0, false); 
    paneSplitter->setCollapsible(1, false); 

    paneSplitter->setStyleSheet(
        "QSplitter::handle {"
        "   background: darkGray;" 
        "   width: 1px;"        
        "   height: 1px;"       
        "   margin: 0px;"       
        "   border: none;"      
        "}"
    );

    OutputPanePlaceHolder* outputPane = new Core::OutputPanePlaceHolder(m_modeStack->getSplitter(), Constants::MAIN_WINDOW);
    outputPane->setObjectName("LogPanePlaceHolder");
    setCentralWidget(paneSplitter);
    setWindowIcon(QIcon(":/image/images/mlico.ico"));

	connect(qApp, &QApplication::focusChanged, this, &MainWindow::updateFocusWidget);
    StatusBarManager::addStatusBarWidget(m_toggleRightSideBarButton, StatusBarManager::RightCorner);
    statusBar()->setProperty("p_styled", true);
    registerDefaultContainers();
    registerDefaultActions();
    QList<IContext*> contexts;
    Context t(Constants::C_GLOBAL);
    IContext* it = new IContext;
    it->setContext(t);
    contexts.append(it);
    updateContextObject(contexts);
    LogManager::create();
    m_modeStack->addLogViewer(outputPane);
    m_modeManager = new ModeManager(this, m_modeStack);

    initRightDock();

    QTimer::singleShot(0, this, [this]() {
        this->setWindowState(Qt::WindowMaximized);
        this->show();
    });

    initStopBtn();
}

MainWindow::~MainWindow()
{}

void Core::MainWindow::init()
{
}

void Core::MainWindow::extensionsInitialized()
{

    ModeManager::extensionsInitialized();
}

void Core::MainWindow::aboutToShutdown()
{
    disconnect(qApp, &QApplication::focusChanged, this, &MainWindow::updateFocusWidget);
    for (auto contextPair : m_contextWidgets)
        disconnect(contextPair.second, &QObject::destroyed, this, nullptr);
    m_activeContext.clear();
    hide();
}
void Core::MainWindow::addContextObject(IContext* context)
{
    if (!context)
        return;
    QWidget* widget = context->widget();
    if (m_contextWidgets.find(widget) != m_contextWidgets.end())
        return;

    m_contextWidgets.insert(std::make_pair(widget, context));
    connect(context, &QObject::destroyed, this, [this, context] { removeContextObject(context); });
}
void Core::MainWindow::updateFocusWidget(QWidget* old, QWidget* now)
{
    Q_UNUSED(old)

        // Prevent changing the context object just because the menu or a menu item is activated
        if (qobject_cast<QMenuBar*>(now) || qobject_cast<QMenu*>(now))
            return;

}

void MainWindow::removeContextObject(IContext* context)
{
    if (!context)
        return;

    disconnect(context, &QObject::destroyed, this, nullptr);

    const auto it = std::find_if(m_contextWidgets.cbegin(),
        m_contextWidgets.cend(),
        [context](const std::pair<QWidget*, IContext*>& v) {
            return v.second == context;
        });
    if (it == m_contextWidgets.cend())
        return;

    m_contextWidgets.erase(it);
    if (m_activeContext.removeAll(context) > 0)
        updateContextObject(m_activeContext);
}
void MainWindow::updateContextObject(const QList<IContext*>& context)
{
    emit m_coreImpl->contextAboutToChange(context);
    m_activeContext = context;
    updateContext();
    if (false) {
        qDebug() << "new context objects =" << context;
        foreach(IContext * c, context)
            qDebug() << (c ? c->widget() : nullptr) << (c ? c->widget()->metaObject()->className() : nullptr);
    }
}
IContext* Core::MainWindow::contextObject(QWidget* widget) const
{
    const auto it = m_contextWidgets.find(widget);
    return it == m_contextWidgets.end() ? nullptr : it->second;
}

void Core::MainWindow::addToolBox(QWidget* wdg, QString toolName)
{
    ads::CDockWidget* dockTab = new ads::CDockWidget(toolName);
    //dockTab->setFeature(ads::CDockWidget::CustomCloseHandling, true);
    dockTab->setContextMenuPolicy(Qt::NoContextMenu);
    dockTab->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    dockTab->setFeature(ads::CDockWidget::NoDockWidgetFeatures, false);
    dockTab->setFeature(ads::CDockWidget::DockWidgetForceCloseWithArea, true);
    //disconnect(dockTab, SIGNAL(closeRequested()), this, SLOT(closeWidget()));
    //connect(dockTab, SIGNAL(closeRequested()), this, SLOT(closeWidget()));
    dockTab->setWidget(wdg);
    dockTab->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    dockTab->setFeature(ads::CDockWidget::NoTab, false);

    m_docks.append(dockTab);

    // °´×ÖµäË³ÐòÅÅÐò
    std::sort(m_docks.begin(), m_docks.end(),
        [](ads::CDockWidget* a, ads::CDockWidget* b) {
            return a->windowTitle().compare(b->windowTitle(), Qt::CaseInsensitive) < 0;
        });

    m_Toolbox++;

    {
        emit finishDock(m_docks);
    }
}

void Core::MainWindow::addMenuAction(QAction* action, QString id, QString parentId)
{
    action->setMenuRole(QAction::PreferencesRole);
    action->setCheckable(false);

    ActionContainer* mtools = ActionManager::actionContainer(parentId);
    Command* cmdT = ActionManager::registerAction(action, id);
    cmdT->setAttribute(Command::CA_UpdateText);
    mtools->addAction(cmdT, Constants::M_TOOL_VIEWS); 
}

void Core::MainWindow::addMenuAction(QAction* action, QString id, QString parentId, QString groupId)
{
    action->setMenuRole(QAction::PreferencesRole);
    action->setCheckable(false);

    ActionContainer* mtools = ActionManager::actionContainer(parentId);
    Command* cmdT = ActionManager::registerAction(action, id);
    cmdT->setAttribute(Command::CA_UpdateText);
    mtools->addAction(cmdT, groupId);
}

static void setRestart(bool restart)
{
    qApp->setProperty("restart", restart);
}


void Core::MainWindow::restart()
{
    setRestart(true);
    exit();
}

void Core::MainWindow::addPreCloseListener(const std::function<bool()>& listener)
{
    m_preCloseListeners.append(listener);
}

void Core::MainWindow::initRightDock()
{
    m_dockManager = new ads::CDockManager();
    m_dockManager->setMinimumWidth(450);
    m_dockManager->setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    m_dockManager->setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    m_dockManager->setContextMenuPolicy(Qt::NoContextMenu);
    m_dockManager->ads::CDockManager::setConfigFlag(ads::CDockManager::ActiveTabHasCloseButton, false);
    m_rightPaneWidget->setWidget(m_dockManager);

    connect(timer, SIGNAL(timeout()), this, SLOT(watchAndHandleRight()));
    timer->start(1000);

    connect(this, SIGNAL(finishDock(QList<ads::CDockWidget*>)), this, SLOT(setCurrent(QList<ads::CDockWidget*>)));

    connect(m_coreImpl, &ICore::coreAboutToClose, [=]() {
        if (m_dockManager)
        {
            m_dockManager->deleteLater();
            m_dockManager = nullptr;
        }
        });
}

bool Core::MainWindow::isAllFloating()
{
    int size = m_docks.size();
    int count = 0;
    int index = 0;
    while (index < size)
    {
        if (m_docks[index]->isFloating() || m_docks[index]->isInFloatingContainer())
        {
            count++;
        }
        index++;
    }
    if (count == size)
    {
        return true;
    }
    return false;
}

void Core::MainWindow::initStopBtn()
{
    QToolButton* eStopBtn = new QToolButton(m_modeStack);
    eStopBtn->setFixedSize(80, 85);
    eStopBtn->setText("Stop");
    eStopBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QIcon eStopIcon(":/image/images/Stop.ico");
    eStopBtn->setIconSize(QSize(64, 64));
    eStopBtn->setIcon(eStopIcon);
    connect(eStopBtn, &QToolButton::clicked, m_coreImpl, &ICore::emgStopBtnClicked);

    m_modeStack->addCornerWidget(eStopBtn);
}

void Core::MainWindow::closeEvent(QCloseEvent* event)
{
    const auto cancelClose = [event]{
        event->ignore();
        setRestart(false);
    };

    // work around QTBUG-43344
    static bool alreadyClosed = false;
    if (alreadyClosed) {
        event->accept();
        return;
    }

    if (m_askConfirmationBeforeExit &&
        (QMessageBox::question(this,
            tr("Exit"),
            tr("Do you really want to exit the application?"),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No)
            == QMessageBox::No)) {
        event->ignore();
        return;
    }

    for(const std::function<bool()> &listener: m_preCloseListeners) {
        if (!listener()) {
            cancelClose();
            return;
        }
    }

    emit m_coreImpl->coreAboutToClose();

    event->accept();
    alreadyClosed = true;
}

void Core::MainWindow::setCurrent(QList<ads::CDockWidget*> docks)
{
    QList<ads::CDockWidget*> reDocks;
    int index = 0;

    for (; index < docks.size(); index++)
    {       
         reDocks.append(docks[index]);
    }
    index = 0;
    for (; index < docks.size(); index++)
    {
        if (!index)
        {
            dockPageManager = m_dockManager->addDockWidget(ads::CenterDockWidgetArea, reDocks[index]);
        }
        else
        {
            m_dockManager->addDockWidget(ads::CenterDockWidgetArea, reDocks[index], dockPageManager);
        }
    }
    reDocks[0]->setAsCurrentTab();
}

void Core::MainWindow::watchAndHandleRight()
{
    if (m_Toolbox < m_docks.size() || !(m_rightPaneWidget->isShown()))
    {
        return;
    }
    if (floatingFlag)
    {
        m_rightPaneWidget->setShown(true);

        if (isAllFloating())
        {
            return;
        }
        floatingFlag = false;
    }
    if (isAllFloating())
    {
        m_rightPaneWidget->setShown(false);
        floatingFlag = true;
    }
}

void Core::MainWindow::exit()
{
    QMetaObject::invokeMethod(this, &QWidget::close, Qt::QueuedConnection);
}

void Core::MainWindow::updateAdditionalContexts(const Context& remove, const Context& add, ICore::ContextPriority priority)
{
    foreach(const QString id, remove) {
        if (!id.isEmpty())
            continue;
        int index = m_lowPrioAdditionalContexts.indexOf(id);
        if (index != -1)
            m_lowPrioAdditionalContexts.removeAt(index);
        index = m_highPrioAdditionalContexts.indexOf(id);
        if (index != -1)
            m_highPrioAdditionalContexts.removeAt(index);
    }

    foreach(const QString id, add) {
        if (!id.isEmpty())
            continue;
        Context& cref = (priority == ICore::ContextPriority::High ? m_highPrioAdditionalContexts
            : m_lowPrioAdditionalContexts);
        if (!cref.contains(id))
            cref.prepend(id);
    }

    updateContext();
}

void Core::MainWindow::registerDefaultContainers()
{
    ActionContainer* menubar = ActionManager::createMenuBar(Constants::MENU_BAR);
    setMenuBar(menubar->menuBar());
    menubar->appendGroup(Constants::G_FILE);
    menubar->appendGroup(Constants::G_TOOLS);
    menubar->appendGroup(Constants::G_HELP);

    // Tools Menu
    ActionContainer* ac = ActionManager::createMenu(Constants::M_TOOLS);
    menubar->addMenu(ac, Constants::G_TOOLS);
    ac->menu()->setTitle(tr("&Tools"));
    ac->appendGroup(Constants::M_TOOL_VIEWS);

    // Help Menu
    ac = ActionManager::createMenu(Constants::M_HELP);
    menubar->addMenu(ac, Constants::G_HELP);
    ac->menu()->setTitle(tr("&Help"));
    ac->appendGroup(Constants::G_HELP_HELP);
    ac->appendGroup(Constants::G_HELP_ABOUT);
}

void Core::MainWindow::registerDefaultActions()
{
    ActionContainer* mfile = ActionManager::actionContainer(Constants::M_FILE);
    ActionContainer* mtools = ActionManager::actionContainer(Constants::M_TOOLS);
    QAction* m_toggleRightSideBarAction = new QAction("Show Right Sidebar", this);
    m_toggleRightSideBarAction->setCheckable(true);
    Command* cmd = ActionManager::registerAction(m_toggleRightSideBarAction, Constants::TOGGLE_RIGHT_SIDEBAR);
    cmd->setAttribute(Command::CA_UpdateText);
    cmd->setDefaultKeySequence(QKeySequence(useMacShortcuts ? tr("Ctrl+Shift+0") : tr("Alt+Shift+0")));
    connect(m_toggleRightSideBarAction, &QAction::triggered,
        this, [this](bool visible) { setSidebarVisible(visible, Side::Right); });
    ProxyAction* toggleRightSideBarProxyAction =
        ProxyAction::proxyActionWithIcon(cmd->action(),QIcon(":/image/images/rightsidebaricon.png"));
    m_toggleRightSideBarButton->setDefaultAction(toggleRightSideBarProxyAction);
    mtools->addAction(cmd, Constants::M_TOOL_VIEWS);
    m_toggleRightSideBarButton->setEnabled(true);
}
void Core::MainWindow::setSidebarVisible(bool visible, Side side)
{
    //ModeManager::activateMode(Id(Constants::MODE_VIDEO));
    panePlaceHolder->currentModeChanged(Constants::MAIN_WINDOW);
    m_rightPaneWidget->setShown(visible);
}
void MainWindow::updateContext()
{
    Context contexts = m_highPrioAdditionalContexts;

    foreach(IContext * context, m_activeContext)
        contexts.add(context->context());

    contexts.add(m_lowPrioAdditionalContexts);

    Context uniquecontexts;
    for (const QString& id : qAsConst(contexts)) {
        if (!uniquecontexts.contains(id))
            uniquecontexts.add(id);
    }

    ActionManager::setContext(uniquecontexts);
    emit m_coreImpl->contextChanged(uniquecontexts);
}
void MainWindow::raiseWindow() {
    setWindowState(windowState() & ~Qt::WindowMinimized);

    raise();

    activateWindow();
}