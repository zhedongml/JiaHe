#pragma once

#include "ui_RecipeEngineWidget.h"

#include <QDir>
#include <QFuture>
#include <QMutex>
#include <QObject>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QStandardpaths>
#include <QVariant>
#include <QWaitCondition>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>

#include "Core/icore.h"
#include "PrjCommon/irecipewrapper.h"
#include "PrjCommon/logindata.h"
#include "PrjCommon/recipewrapperregistry.h"


#include "DispatchRecipeModule.h"
#include "RecipeDataModules.h"
#include "PrjCommon/Session.h"
#include <QFutureWatcher>

typedef struct
{
    int cycleCount;
    bool forIn;
    QMap<QString, QList<QString>> paraListMap;
    QMap<QString, QString> globalParaMap;
    QList<QString> recipeUpList;
    QList<QString> recipeDownList;

} RecipeCycleInfo;

class RecipeEngineWidget : public QQuickWidget
{
    Q_OBJECT
    Q_PROPERTY(bool status READ getStatus WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString file READ getFileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(bool mode READ getMode WRITE setMode NOTIFY modeChanged)

    Q_PROPERTY(bool pause READ getPause WRITE setPause NOTIFY statusChanged)

    Q_PROPERTY(bool operatorMode READ getIsOperator NOTIFY operateChanged)

  public:
    Q_INVOKABLE QString UrlToNativeFilePath(const QUrl &url) const;
    Q_INVOKABLE void saveCurrentContextVar(QString value);
    Q_INVOKABLE bool saveResults(const QString &path);
    Q_INVOKABLE void recipeStarted();
    Q_INVOKABLE void recipeEnded();
    Q_INVOKABLE void recipeStopped();
    Q_INVOKABLE void recipeClapEnded();

    RecipeEngineWidget(DispatchRecipeModule *m_dispatchModule, QWidget *parent = Q_NULLPTR);
    ~RecipeEngineWidget();
    bool getStatus();
    void setStatus(bool s);
    QString getFileName();
    void setFileName(QString newName);
    bool getMode();
    void setMode(bool newmode);
    void delayAndExecute(int, QString);

    bool getPause();
    void setPause(bool val);
    bool getIsOperator();

  signals:
    void disableQml();
    void finishedLoadingModulesQml();
    void moduleFailedQml(QString msg);
    void modulePassedQml();
    void userResponseQml(QString load, QString msg, bool retrieveContext);
    void doneQml();
    void errorQml(bool fatal, QString msg);
    void statusChanged();
    void fileNameChanged();
    void modeChanged();
    void emgStop();
    void operateChanged();
  public slots:
    void onExecuteModule(QString module);
    void onStartLoadingModules();
    void onResponse(CORE::WrapperStatus, QString);
    void initiateExecution(QString);
    void emgStopRecipe();
    void onWaitResTimeOut();
    void threadFinished();

  private:
    void readFixRecipeFile();
    bool operatorMode = false;
    void readFile();
    bool parseModule(const QString &, QStringList &fields);
    bool parseString(int &index, const QString &, QStringList &fields, bool isParameter = false);
    void analysisForRecipe(QList<QString> list);
    void analysisForRecipeNew(QList<QString> list);
    void insertRecipe(QList<QString> recipeList, QMap<QString, QList<QString>> paraMap,
                      QMap<QString, QString> globalParaMap, int cyc);
    void recursiveRecipe(QList<RecipeCycleInfo> cycleList, int floor, int count);
    void forRecipe(int curIndex);
    void loadGlobalPara(QList<RecipeCycleInfo> &cycleList, int floor, int cyc);
    bool condition(const QString &line);
    Result judgeLine(const QString &line, int number);

  private:
    Ui::RecipeEngineWidget ui;
    DispatchRecipeModule *m_dispatchModule;
    RecipeDataModules m_modules;

    bool m_IsRunning = false;
    QString m_currentFile;       // stores current module opened file
    QString m_currentContextVar; // stores current custom context variable
    bool m_mode = false;          // stores current recipe mode
    int m_moduleDelay = 100;     // number of seconds to delay befor executing next module
    void updateContextVar(QString value);
    bool isPause = false;
    // QFuture<void> future;

    QWaitCondition waitObj;
    QMutex mutex;

    QTimer *m_waitResTimer;
    int m_waitMsec = 120 * 60 * 1000; // default 3 min
    bool canEndSignal = false;

    QFutureWatcher<void> m_watcher;
    bool m_threadStart = false;

    QMap<int, QList<QString>> m_forLineMap;
    QMap<int, QMap<QString, QList<QString>>> m_forInfoMap;
};
