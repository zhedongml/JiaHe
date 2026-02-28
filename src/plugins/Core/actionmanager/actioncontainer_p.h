#pragma once

#include "actionmanager_p.h"

#include "actioncontainer.h"
#include "command.h"


namespace Core {

struct Group
{
    Group(QString id) : id(id) {}
    QString id;
    QList<QObject *> items; // Command * or ActionContainer *
};

class ActionContainerPrivate : public ActionContainer
{
    Q_OBJECT

public:
    ActionContainerPrivate(QString id);
    ~ActionContainerPrivate() override = default;

    void setOnAllDisabledBehavior(OnAllDisabledBehavior behavior) override;
    ActionContainer::OnAllDisabledBehavior onAllDisabledBehavior() const override;

    QAction *insertLocation(QString groupId) const override;
    void appendGroup(QString id) override;
    void insertGroup(QString before, QString groupId) override;
    void addAction(Command *action, QString group = {}) override;
    void addMenu(ActionContainer *menu, QString group = {}) override;
    void addMenu(ActionContainer *before, ActionContainer *menu) override;
    Command *addSeparator(const Context &context, QString group = {}, QAction **outSeparator = nullptr) override;
    void clear() override;

    QString id() const override;

    QMenu *menu() const override;
    QMenuBar *menuBar() const override;
    
    virtual QAction *containerAction() const = 0;
    virtual QAction *actionForItem(QObject *item) const;

    virtual void insertAction(QAction *before, Command *command) = 0;
    virtual void insertMenu(QAction *before, ActionContainer *container) = 0;

    virtual void removeAction(Command *command) = 0;
    virtual void removeMenu(ActionContainer *container) = 0;

    virtual bool updateInternal() = 0;

protected:
    static bool canAddAction(Command *action);
    bool canAddMenu(ActionContainer *menu) const;
    virtual bool canBeAddedToContainer(ActionContainerPrivate *container) const = 0;

    // groupId --> list of Command* and ActionContainer*
    QList<Group> m_groups;

private:
    void scheduleUpdate();
    void update();
    void itemDestroyed();

    QList<Group>::const_iterator findGroup(QString groupId) const;
    QAction *insertLocation(QList<Group>::const_iterator group) const;

    OnAllDisabledBehavior m_onAllDisabledBehavior;
    QString m_id;
    bool m_updateRequested;
};

class MenuActionContainer : public ActionContainerPrivate
{
    Q_OBJECT

public:
    explicit MenuActionContainer(QString id);
    ~MenuActionContainer() override;

    QMenu *menu() const override;

    QAction *containerAction() const override;

    void insertAction(QAction *before, Command *command) override;
    void insertMenu(QAction *before, ActionContainer *container) override;

    void removeAction(Command *command) override;
    void removeMenu(ActionContainer *container) override;

protected:
    bool canBeAddedToContainer(ActionContainerPrivate *container) const override;
    bool updateInternal() override;

private:
    QPointer<QMenu> m_menu;
};

class MenuBarActionContainer : public ActionContainerPrivate
{
    Q_OBJECT

public:
    explicit MenuBarActionContainer(QString id);

    void setMenuBar(QMenuBar *menuBar);
    QMenuBar *menuBar() const override;

    QAction *containerAction() const override;

    void insertAction(QAction *before, Command *command) override;
    void insertMenu(QAction *before, ActionContainer *container) override;

    void removeAction(Command *command) override;
    void removeMenu(ActionContainer *container) override;

protected:
    bool canBeAddedToContainer(ActionContainerPrivate *container) const override;
    bool updateInternal() override;

private:
    QMenuBar *m_menuBar;
};

} // namespace Core
