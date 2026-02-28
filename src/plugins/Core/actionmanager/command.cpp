#include "command_p.h"

#include "../coreconstants.h"
#include "../icontext.h"
#include "../proxyaction.h"
#include <QAction>
#include <QToolButton>
#include <QTextStream>

/*!
    \class Core::Command
    \inheaderfile coreplugin/actionmanager/command.h
    \ingroup mainclasses

    \brief The Command class represents an action, such as a menu item, tool button, or shortcut.

    You do not create Command objects directly, but use \l{ActionManager::registerAction()}
    to register an action and retrieve a Command. The Command object represents the user visible
    action and its properties. If multiple actions are registered with the same ID (but
    different contexts) the returned Command is the shared one between these actions.

    A Command has two basic properties: a list of default shortcuts and a
    default text. The default shortcuts are key sequence that the user can use
    to trigger the active action that the Command represents. The first
    shortcut in that list is the main shortcut that is for example also shown
    in tool tips and menus. The default text is used for representing the
    Command in the keyboard shortcut preference pane. If the default text is
    empty, the text of the visible action is used.

    The user visible action is updated to represent the state of the active action (if any).
    For performance reasons only the enabled and visible state are considered by default though.
    You can tell a Command to also update the actions icon and text by setting the
    corresponding \l{Command::CommandAttribute}{attribute}.

    If there is no active action, the default behavior of the visible action is to be disabled.
    You can change that behavior to make the visible action hide instead via the Command's
    \l{Command::CommandAttribute}{attributes}.

    See \l{The Action Manager and Commands} for an overview of how
    Core::Command and Core::ActionManager interact.

    \sa Core::ActionManager
    \sa {The Action Manager and Commands}
*/

/*!
    \enum Core::Command::CommandAttribute
    This enum defines how the user visible action is updated when the active action changes.
    The default is to update the enabled and visible state, and to disable the
    user visible action when there is no active action.
    \value CA_UpdateText
        Also update the actions text.
    \value CA_UpdateIcon
        Also update the actions icon.
    \value CA_Hide
        When there is no active action, hide the user-visible action, instead of just
        disabling it.
    \value CA_NonConfigurable
        Flag to indicate that the keyboard shortcuts of this Command should not
        be configurable by the user.
*/

/*!
    \fn void Core::Command::setDefaultKeySequence(const QKeySequence &key)

    Sets the default keyboard shortcut that can be used to activate this
    command to \a key. This is used if the user didn't customize the shortcut,
    or resets the shortcut to the default.
*/

/*!
    \fn void Core::Command::setDefaultKeySequences(const QList<QKeySequence> &keys)

    Sets the default keyboard shortcuts that can be used to activate this
    command to \a keys. This is used if the user didn't customize the
    shortcuts, or resets the shortcuts to the default.
*/

/*!
    \fn QList<QKeySequence> Core::Command::defaultKeySequences() const

    Returns the default keyboard shortcuts that can be used to activate this
    command.
    \sa setDefaultKeySequences()
*/

/*!
    \fn void Core::Command::keySequenceChanged()
    Sent when the keyboard shortcuts assigned to this Command change, e.g.
    when the user sets them in the keyboard shortcut settings dialog.
*/

/*!
    \fn QList<QKeySequence> Core::Command::keySequences() const

    Returns the current keyboard shortcuts assigned to this Command.
    \sa defaultKeySequences()
*/

/*!
    \fn QKeySequence Core::Command::keySequence() const

    Returns the current main keyboard shortcut assigned to this Command.
    \sa defaultKeySequences()
*/

/*!
    \fn void Core::Command::setKeySequences(const QList<QKeySequence> &keys)
    \internal
*/

/*!
    \fn void Core::Command::setDescription(const QString &text)
    Sets the \a text that is used to represent the Command in the
    keyboard shortcut settings dialog. If you do not set this,
    the current text from the user visible action is taken (which
    is fine in many cases).
*/

/*!
    \fn QString Core::Command::description() const
    Returns the text that is used to present this Command to the user.
    \sa setDescription()
*/

/*!
    \fn int Core::Command::id() const
    \internal
*/

/*!
    \fn QString Core::Command::stringWithAppendedShortcut(const QString &string) const

    Returns the \a string with an appended representation of the main keyboard
    shortcut that is currently assigned to this Command.
*/

/*!
    \fn QAction *Core::Command::action() const

    Returns the user visible action for this Command. Use this action to put it
    on e.g. tool buttons. The action automatically forwards \c triggered() and
    \c toggled() signals to the action that is currently active for this
    Command. It also shows the current main keyboard shortcut in its tool tip
    (in addition to the tool tip of the active action) and gets disabled/hidden
    when there is no active action for the current context.
*/

/*!
    \fn Core::Context Core::Command::context() const

    Returns the context for this command.
*/

/*!
    \fn void Core::Command::setAttribute(Core::Command::CommandAttribute attribute)
    Adds \a attribute to the attributes of this Command.
    \sa CommandAttribute
    \sa removeAttribute()
    \sa hasAttribute()
*/

/*!
    \fn void Core::Command::removeAttribute(Core::Command::CommandAttribute attribute)
    Removes \a attribute from the attributes of this Command.
    \sa CommandAttribute
    \sa setAttribute()
*/

/*!
    \fn bool Core::Command::hasAttribute(Core::Command::CommandAttribute attribute) const
    Returns whether the Command has the \a attribute set.
    \sa CommandAttribute
    \sa removeAttribute()
    \sa setAttribute()
*/

/*!
    \fn bool Core::Command::isActive() const

    Returns whether the Command has an active action for the current context.
*/

/*!
    \fn bool Core::Command::isScriptable() const
    Returns whether the Command is scriptable. A scriptable command can be called
    from a script without the need for the user to interact with it.
*/

/*!
    \fn bool Core::Command::isScriptable(const Core::Context &) const
    \internal

    Returns whether the Command is scriptable.
*/

/*!
    \fn void Core::Command::activeStateChanged()

    This signal is emitted when the active state of the command changes.
*/

/*!
    \fn virtual void Core::Command::setTouchBarText(const QString &text)

    Sets the text for the action on the touch bar to \a text.
*/

/*!
    \fn virtual QString Core::Command::touchBarText() const

    Returns the text for the action on the touch bar.
*/

/*!
    \fn virtual void Core::Command::setTouchBarIcon(const QIcon &icon)

    Sets the icon for the action on the touch bar to \a icon.
*/

/*! \fn virtual QIcon Core::Command::touchBarIcon() const

    Returns the icon for the action on the touch bar.
*/

/*! \fn virtual QAction *Core::Command::touchBarAction() const

    \internal
*/

namespace Core {

Action::Action(QString id)
    : m_attributes({}),
      m_id(id),
      m_action(new  ProxyAction(this))
{
    connect(m_action, &QAction::changed, this, &Action::updateActiveState);
}

QString Action::id() const
{
    return m_id;
}

void Action::setDefaultKeySequence(const QKeySequence &key)
{
    if (!m_isKeyInitialized)
        setKeySequences({key});
    m_defaultKeys = {key};
}

void Action::setDefaultKeySequences(const QList<QKeySequence> &keys)
{
    if (!m_isKeyInitialized)
        setKeySequences(keys);
    m_defaultKeys = keys;
}

QList<QKeySequence> Action::defaultKeySequences() const
{
    return m_defaultKeys;
}

QAction *Action::action() const
{
    return m_action;
}

Context Action::context() const
{
    return m_context;
}

void Action::setKeySequences(const QList<QKeySequence> &keys)
{
    m_isKeyInitialized = true;
    if (m_action) {
        m_action->setShortcuts(keys);
        emit keySequenceChanged();
    }
}

QList<QKeySequence> Action::keySequences() const
{
    return m_action->shortcuts();
}

QKeySequence Action::keySequence() const
{
    return m_action->shortcut();
}

void Action::setDescription(const QString &text)
{
    m_defaultText = text;
}

QString Action::description() const
{
    if (!m_defaultText.isEmpty())
        return m_defaultText;
    if (QAction *act = action()) {
        const QString text = act->text();
        if (!text.isEmpty())
            return text;
    }
    return id();
}

void Action::setCurrentContext(const Context &context)
{
    m_context = context;

    QAction *currentAction = nullptr;
    for (int i = 0; i < m_context.size(); ++i) {
        if (QAction *a = m_contextActionMap.value(m_context.at(i), nullptr)) {
            currentAction = a;
            break;
        }
    }
    m_action->setAction(currentAction);
    updateActiveState();
}

void Action::updateActiveState()
{
    if(m_action)
     setActive(m_action->isEnabled() && m_action->isVisible() && !m_action->isSeparator());
}

static QString msgActionWarning(QAction *newAction, QString id, QAction *oldAction)
{
    QString msg;
    QTextStream str(&msg);
    str << "addOverrideAction " << newAction->objectName() << '/' << newAction->text()
         << ": Action ";
    if (oldAction)
        str << oldAction->objectName() << '/' << oldAction->text();
    str << " is already registered for context " << id<< '.';
    return msg;
}

void Action::addOverrideAction(QAction* action, const Context& context, bool scriptable)
{
    // disallow TextHeuristic menu role, because it doesn't work with translations,
    // e.g. QTCREATORBUG-13101
    if (action->menuRole() == QAction::TextHeuristicRole)
        action->setMenuRole(QAction::NoRole);
    if (isEmpty())
        m_action->initialize(action);
    if (context.isEmpty()) {
        m_contextActionMap.insert(Constants::C_GLOBAL, action);
    }
    else {
        for (const QString& id : context) {
            if (m_contextActionMap.contains(id))
                qWarning("%s", qPrintable(msgActionWarning(action, id, m_contextActionMap.value(id, nullptr))));
            m_contextActionMap.insert(id, action);
        }
    }
    m_scriptableMap[action] = scriptable;
    setCurrentContext(m_context);
}

void Action::removeOverrideAction(QAction* action)
{
    QList<QString> toRemove;
    for (auto it = m_contextActionMap.cbegin(), end = m_contextActionMap.cend(); it != end; ++it) {
        if (it.value() == nullptr || it.value() == action)
            toRemove.append(it.key());
    }
    for (QString id : toRemove)
        m_contextActionMap.remove(id);
    setCurrentContext(m_context);
}
bool Action::isActive() const
{
    return m_active;
}

void Action::setActive(bool state)
{
    if (state != m_active) {
        m_active = state;
        emit activeStateChanged();
    }
}

bool Action::isEmpty() const
{
    return m_contextActionMap.isEmpty();
}

bool Action::isScriptable() const
{
    return std::find(m_scriptableMap.cbegin(), m_scriptableMap.cend(), true) !=
            m_scriptableMap.cend();
}


void Action::setAttribute(CommandAttribute attr)
{
    m_attributes |= attr;
    switch (attr) {
    case Command::CA_Hide:
        m_action->setAttribute(ProxyAction::Hide);
        break;
    case Command::CA_UpdateText:
        m_action->setAttribute(ProxyAction::UpdateText);
        break;
    case Command::CA_UpdateIcon:
        m_action->setAttribute(ProxyAction::UpdateIcon);
        break;
    case Command::CA_NonConfigurable:
        break;
    }
}

void Action::removeAttribute(CommandAttribute attr)
{
    m_attributes &= ~attr;
    switch (attr) {
    case Command::CA_Hide:
        m_action->removeAttribute(ProxyAction::Hide);
        break;
    case Command::CA_UpdateText:
        m_action->removeAttribute(ProxyAction::UpdateText);
        break;
    case Command::CA_UpdateIcon:
        m_action->removeAttribute(ProxyAction::UpdateIcon);
        break;
    case Command::CA_NonConfigurable:
        break;
    }
}
bool Action::hasAttribute(Command::CommandAttribute attr) const
{
    return (m_attributes & attr);
}
} // namespace Core
