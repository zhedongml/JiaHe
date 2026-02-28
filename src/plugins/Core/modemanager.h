#pragma once

#include "core_global.h"


#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace Core {

 class IMode;
 class MainWindow;
 class FancyTabWidget;

class CORE_EXPORT ModeManager : public QObject
{
    Q_OBJECT

public:
    enum class Style {
        IconsAndText,
        IconsOnly,
        Hidden
    };

    static ModeManager *instance();

    static IMode *currentMode();
    static QString currentModeId();

    static void addAction(QAction *action, int priority);

    static void activateMode(QString id);
    static void setFocusToCurrentMode();
    static Style modeStyle();

    static void removeMode(IMode *mode);

    static void addWidgetToStack(QWidget* widget, int index);

    static IMode* findMode(QString id);

public slots:
    static void setModeStyle(Style layout);
    static void cycleModeStyle();

signals:
    void currentModeAboutToChange(QString mode);

    // the default argument '=0' is important for connects without the oldMode argument.
    void currentModeChanged(QString mode, QString oldMode = {});

private:
    explicit ModeManager( MainWindow *mainWindow, FancyTabWidget *modeStack);
    ~ModeManager() override;

    static void extensionsInitialized();

    static void addMode(IMode *mode);
    void currentTabAboutToChange(int index);
    void currentTabChanged(int index);

    friend class IMode;
    friend class Core::MainWindow;
};

} // namespace Core
