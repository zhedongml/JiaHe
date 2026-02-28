#pragma once

#include "../core_global.h"
#include "../icontext.h"
#include "command.h"
#include "../coreconstants.h"
#include <QObject>
#include <QList>

QT_BEGIN_NAMESPACE
class QAction;
class QString;
QT_END_NAMESPACE

namespace Core {

class ActionContainer;
class Command;
class Context;

class CorePlugin;
class MainWindow;


class CORE_EXPORT ActionManager : public QObject
{
    Q_OBJECT
public:
    static ActionManager *instance();

    static ActionContainer *createMenu(QString id);
    static ActionContainer *createMenuBar(QString id);


    static Command *registerAction(QAction *action, QString id,
                                   const Context &context = Context(Constants::C_GLOBAL),
                                   bool scriptable = false);

    static Command *command(QString id);
    static ActionContainer *actionContainer(QString id);

    static QList<Command *> commands();

    static void unregisterAction(QAction *action, QString id);

    static void setPresentationModeEnabled(bool enabled);
    static bool isPresentationModeEnabled();

    static QString withNumberAccelerator(const QString &text, const int number);

signals:
    void commandListChanged();
    void commandAdded(QString id);

private:
    ActionManager(QObject *parent = nullptr);
    ~ActionManager() override;
    static void setContext(const Context &context);

    friend class Core::CorePlugin; // initialization
    friend class Core::MainWindow; // saving settings and setting context
};

} // namespace Core
