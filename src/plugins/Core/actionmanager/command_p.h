#pragma once

#include "command.h"

#include "../icontext.h"
#include "../proxyaction.h"
#include <QList>
#include <QMultiMap>
#include <QPointer>
#include <QMap>
#include <QKeySequence>

#include <memory>
#include <QIcon>
namespace Core {

class Action : public Command
{
    Q_OBJECT
public:
    Action(QString id);

    QString id() const override;

    void setDefaultKeySequence(const QKeySequence &key) override;
    void setDefaultKeySequences(const QList<QKeySequence> &key) override;
    QList<QKeySequence> defaultKeySequences() const override;

    void setKeySequences(const QList<QKeySequence> &keys) override;
    QList<QKeySequence> keySequences() const override;
    QKeySequence keySequence() const override;

    void setDescription(const QString &text) override;
    QString description() const override;

    QAction *action() const override;

    Context context() const override;
    void setCurrentContext(const Context &context);

    bool isActive() const override;
    void addOverrideAction(QAction* action, const Context& context, bool scriptable);
    void removeOverrideAction(QAction* action);
    bool isEmpty() const;

    bool isScriptable() const override;
    void setAttribute(CommandAttribute attr) override;
    void removeAttribute(CommandAttribute attr) override;
    bool hasAttribute(CommandAttribute attr) const override;


private:
    void updateActiveState();
    void setActive(bool state);

    Context m_context;
    CommandAttributes m_attributes;
    QString m_id;
    QList<QKeySequence> m_defaultKeys;
    QString m_defaultText;
    QString m_touchBarText;
    QIcon m_touchBarIcon;
    bool m_isKeyInitialized = false;

    QString m_toolTip;

    QMap<QString, QPointer<QAction> > m_contextActionMap;
    QMap<QAction*, bool> m_scriptableMap;
    bool m_active = false;
    bool m_contextInitialized = false;
    ProxyAction* m_action = nullptr;
};
} // namespace Core
