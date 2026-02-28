#pragma once

#include "../core_global.h"
#include "../icontext.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QMenu;
class QMenuBar;
class QAction;
QT_END_NAMESPACE

namespace Core {

class Command;

class CORE_EXPORT ActionContainer : public QObject
{
    Q_OBJECT

public:
    enum OnAllDisabledBehavior {
        Disable,
        Hide,
        Show
    };

    virtual void setOnAllDisabledBehavior(OnAllDisabledBehavior behavior) = 0;
    virtual ActionContainer::OnAllDisabledBehavior onAllDisabledBehavior() const = 0;

    virtual QString id() const = 0;

    virtual QMenu *menu() const = 0;
    virtual QMenuBar *menuBar() const = 0;

    virtual QAction *insertLocation(QString group) const = 0;
    virtual void appendGroup(QString group) = 0;
    virtual void insertGroup(QString before, QString group) = 0;
    virtual void addAction(Command *action, QString group = {}) = 0;
    virtual void addMenu(ActionContainer *menu, QString group = {}) = 0;
    virtual void addMenu(ActionContainer *before, ActionContainer *menu) = 0;
    Command *addSeparator(QString group = {});
    virtual Command *addSeparator(const Context &context, QString group = {}, QAction **outSeparator = nullptr) = 0;

    // This clears this menu and submenus from all actions and submenus.
    // It does not destroy the submenus and commands, just removes them from their parents.
    virtual void clear() = 0;
};

} // namespace Core
