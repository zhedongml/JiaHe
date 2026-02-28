/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "modemanager.h"


#include "mainwindow.h"

#include "imode.h"

#include "pluginsystem/pluginmanager.h"
#include "pluginsystem/algorithm.h"
#include "fancytabwidget.h"
#include "icontext.h"
#include <QAction>
#include <QDebug>
#include <QMap>
#include <QMouseEvent>
#include <QVector>
#include "icore.h"
#include "actionmanager/command.h"
#include "actionmanager/actionmanager.h"
namespace Core {

/*!
    \class Core::ModeManager
    \inheaderfile coreplugin/modemanager.h
    \ingroup mainclasses
    \inmodule QtCreator

    \brief The ModeManager class manages the activation of modes and the
    actions in the mode selector's tool bar.

    Modes are implemented with the IMode class. Use the ModeManager to
    force activation of a mode, or to be notified when the active mode changed.

    The ModeManager also manages the actions that are visible in the mode
    selector's toolbar. Adding actions to the tool bar should be done very
    sparingly.
*/

/*!
    \enum ModeManager::Style
    \internal
*/

/*!
    \fn void ModeManager::currentModeAboutToChange(Utils::Id mode)

    Emitted before the current mode changes to \a mode.
*/

/*!
    \fn void ModeManager::currentModeChanged(Utils::Id mode, Utils::Id oldMode)

    Emitted after the current mode changed from \a oldMode to \a mode.
*/

struct ModeManagerPrivate
{
    void showMenu(int index, QMouseEvent *event);
    void appendMode(IMode *mode);
    void enabledStateChanged(IMode *mode);
    void activateModeHelper(QString id);
    void extensionsInitializedHelper();

    MainWindow *m_mainWindow;
    FancyTabWidget *m_modeStack;
    QMap<QAction*, int> m_actions;
    QVector<IMode*> m_modes;
    QVector<Command*> m_modeCommands;
    Context m_addedContexts;
    int m_oldCurrent;
    ModeManager::Style m_modeStyle = ModeManager::Style::IconsAndText;

    bool m_startingUp = true;
    QString m_pendingFirstActiveMode; // Valid before extentionsInitialized.
};

static ModeManagerPrivate *d;
static ModeManager *m_instance = nullptr;

static int indexOf(QString id)
{
    for (int i = 0; i < d->m_modes.count(); ++i) {
        if (d->m_modes.at(i)->id() == id)
            return i;
    }
    qDebug() << "Warning, no such mode:" << id;
    return -1;
}

void ModeManagerPrivate::showMenu(int index, QMouseEvent *event)
{
    m_modes.at(index)->menu()->popup(event->globalPos());
}
void ModeManager::addWidgetToStack(QWidget* widget, int index) {
    d->m_modeStack->addStackWidget(widget, index);
}
ModeManager::ModeManager(MainWindow *mainWindow,
                         FancyTabWidget *modeStack)
{
    m_instance = this;
    d = new ModeManagerPrivate();
    d->m_mainWindow = mainWindow;
    d->m_modeStack = modeStack;
    d->m_oldCurrent = -1;

    setModeStyle(d->m_modeStyle);

    connect(d->m_modeStack, & FancyTabWidget::currentAboutToShow,
            this, &ModeManager::currentTabAboutToChange);
    connect(d->m_modeStack, &FancyTabWidget::currentChanged,
            this, &ModeManager::currentTabChanged);
    connect(d->m_modeStack, & FancyTabWidget::menuTriggered,
            this, [](int index, QMouseEvent *e) { d->showMenu(index, e); });
}

ModeManager::~ModeManager()
{
    delete d;
    d = nullptr;
    m_instance = nullptr;
}

/*!
    Returns the id of the current mode.

    \sa activateMode()
    \sa currentMode()
*/
QString ModeManager::currentModeId()
{
    int currentIndex = d->m_modeStack->currentIndex();
    if (currentIndex < 0)
        return "";
    return d->m_modes.at(currentIndex)->id();
}

IMode* ModeManager::findMode(QString id)
{
    const int index = indexOf(id);
    if (index >= 0)
        return d->m_modes.at(index);
    return nullptr;
}

/*!
    Makes the mode with ID \a id the current mode.

    \sa currentMode()
    \sa currentModeId()
    \sa currentModeAboutToChange()
    \sa currentModeChanged()
*/
void ModeManager::activateMode(QString id)
{
    d->activateModeHelper(id);
}

void ModeManagerPrivate::activateModeHelper(QString id)
{
    if (m_startingUp) {
        m_pendingFirstActiveMode = id;
    } else {
        const int currentIndex = m_modeStack->currentIndex();
        const int newIndex = indexOf(id);
        if (newIndex != currentIndex && newIndex >= 0)
            m_modeStack->setCurrentIndex(newIndex);
    }
}

void ModeManager::extensionsInitialized()
{
    d->extensionsInitializedHelper();
}

void ModeManagerPrivate::extensionsInitializedHelper()
{
    m_startingUp = false;
    Utils::sort(m_modes, &IMode::priority);
    std::reverse(m_modes.begin(), m_modes.end());
    for (IMode *mode : qAsConst(m_modes))
        appendMode(mode);

    if (!m_pendingFirstActiveMode.isEmpty())
        activateModeHelper(m_pendingFirstActiveMode);
}

void ModeManager::addMode(IMode *mode)
{
    if(mode)
      d->m_modes.append(mode);
}

void ModeManagerPrivate::appendMode(IMode* mode)
{
    const int index = m_modeCommands.count();

    m_mainWindow->addContextObject(mode);

    if (mode->widget())
    {
        m_modeStack->insertTab(index, mode->widget(), mode->icon(), mode->displayName(),
            mode->menu() != nullptr);
    }
    else {
        QWidget* logicWidget = mode->logicWidget();
        QWidget* resultWidget = mode->resultWidget();
        if (logicWidget && resultWidget) {
            m_modeStack->insertPartialUpdateTab(index, logicWidget, resultWidget, mode->icon(), mode->displayName(),
            mode->menu() != nullptr);
        }
    }
    //m_modeStack->setTabEnabled(index, mode->isEnabled());

    // Register mode shortcut
    const QString actionId = mode->id().contains("Mode.");
    QAction* action = new QAction(ModeManager::tr("Switch to <b>%1</b> mode").arg(mode->displayName()), m_instance);
    Command* cmd = ActionManager::registerAction(action, actionId);
    cmd->setDefaultKeySequence(QKeySequence(useMacShortcuts ? QString("Meta+%1").arg(index + 1)
        : QString("Ctrl+%1").arg(index + 1)));
    m_modeCommands.append(cmd);

    m_modeStack->setTabToolTip(index, cmd->action()->toolTip());
    QObject::connect(cmd, &Command::keySequenceChanged, m_instance, [cmd, index, this] {
        m_modeStack->setTabToolTip(index, cmd->action()->toolTip());
        });

    QString id = mode->id();
    QObject::connect(action, &QAction::triggered, [this, id] {
        ModeManager::activateMode(id);
        ICore::raiseWindow(m_modeStack);
        });

    QObject::connect(mode, &IMode::enabledStateChanged, [this, mode] { enabledStateChanged(mode); });
}

void ModeManager::removeMode(IMode *mode)
{
    const int index = d->m_modes.indexOf(mode);
    if (index >= d->m_modes.size() - 1 && d->m_modes.size() > 1)
        d->m_modeStack->setCurrentIndex(d->m_modes.size() - 2);
    d->m_modes.remove(index);
    if (d->m_startingUp)
        return;

    d->m_modeStack->removeTab(index);
    d->m_mainWindow->removeContextObject(mode);
}

void ModeManagerPrivate::enabledStateChanged(IMode *mode)
{
    int index = d->m_modes.indexOf(mode);

    // Make sure we leave any disabled mode to prevent possible crashes:
    if (mode->id() == ModeManager::currentModeId() && !mode->isEnabled()) {
        // This assumes that there is always at least one enabled mode.
        for (int i = 0; i < d->m_modes.count(); ++i) {
            if (d->m_modes.at(i) != mode &&
                d->m_modes.at(i)->isEnabled()) {
                ModeManager::activateMode(d->m_modes.at(i)->id());
                break;
            }
        }
    }
}

/*!
    Adds the \a action to the mode selector's tool bar.
    Actions are sorted by \a priority in descending order.
    Use this functionality very sparingly.
*/
void ModeManager::addAction(QAction *action, int priority)
{
    d->m_actions.insert(action, priority);

    // Count the number of commands with a higher priority
    int index = 0;
    foreach (int p, d->m_actions) {
        if (p > priority)
            ++index;
    }

}

void ModeManager::currentTabAboutToChange(int index)
{
    if (index >= 0) {
        IMode *mode = d->m_modes.at(index);
        if (mode)
            emit currentModeAboutToChange(mode->id());
    }
}

void ModeManager::currentTabChanged(int index)
{
    // Tab index changes to -1 when there is no tab left.
    if (index < 0)
        return;

    IMode *mode = d->m_modes.at(index);
    if (!mode)
        return;

    // FIXME: This hardcoded context update is required for the Debug and Edit modes, since
    // they use the editor widget, which is already a context widget so the main window won't
    // go further up the parent tree to find the mode context.
    ICore::updateAdditionalContexts(d->m_addedContexts, mode->context());
    d->m_addedContexts = mode->context();

    IMode *oldMode = nullptr;
    if (d->m_oldCurrent >= 0)
        oldMode = d->m_modes.at(d->m_oldCurrent);
    d->m_oldCurrent = index;
    emit currentModeChanged(mode->id(), oldMode ? oldMode->id() : "");
}

/*!
    \internal
*/
void ModeManager::setFocusToCurrentMode()
{
    IMode *mode = findMode(currentModeId());
    QWidget *widget = mode->widget();
    if (widget) {
        QWidget *focusWidget = widget->focusWidget();
        if (!focusWidget)
            focusWidget = widget;
        focusWidget->setFocus();
    }
}

/*!
    \internal
*/
void ModeManager::setModeStyle(ModeManager::Style style)
{
    const bool visible = style != Style::Hidden;
    const bool iconsOnly = style == Style::IconsOnly;
}

/*!
    \internal
*/
void ModeManager::cycleModeStyle()
{
    auto nextStyle = Style((int(modeStyle()) + 1) % 3);
    setModeStyle(nextStyle);
}

/*!
    \internal
*/
ModeManager::Style ModeManager::modeStyle()
{
    return d->m_modeStyle;
}

/*!
    Returns the pointer to the instance. Only use for connecting to signals.
*/
ModeManager *ModeManager::instance()
{
    return m_instance;
}

/*!
    Returns a pointer to the current mode.

    \sa activateMode()
    \sa currentModeId()
*/
IMode *ModeManager::currentMode()
{
    const int currentIndex = d->m_modeStack->currentIndex();
    return currentIndex < 0 ? nullptr : d->m_modes.at(currentIndex);
}

} // namespace Core
