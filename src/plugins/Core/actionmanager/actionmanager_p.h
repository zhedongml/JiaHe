
#pragma once

#include "command_p.h"
#include "actioncontainer_p.h"
#include "../icontext.h"

#include <QMap>
#include <QHash>
#include <QMultiHash>
#include <QTimer>

namespace Core {
class Action;
class ActionContainerPrivate;

class ActionManagerPrivate : public QObject
{
    Q_OBJECT

public:
    using IdCmdMap = QHash<QString, Action *>;
    using IdContainerMap = QHash<QString, ActionContainerPrivate *>;

    ~ActionManagerPrivate() override;

    void setContext(const Context &context);
    bool hasContext(int context) const;

    static void showShortcutPopup(const QString &shortcut);
    bool hasContext(const Context &context) const;
    Action *overridableAction(QString id);

    void containerDestroyed();
    void actionTriggered();

    IdCmdMap m_idCmdMap;

    IdContainerMap m_idContainerMap;

    Context m_context;

    bool m_presentationModeEnabled = false;
};
} // namespace Core
