#include "RecipeEngineWidget.h"
#include <QtWidgets\qmessagebox.h>
#include <PLCControl\PLCController.h>
#include "PrjCommon/WaitWidget.h"
#include <PrjCommon/metricsdata.h>
#include "Core/loggingwrapper.h"
#include "PrjCommon/PrjCommon.h"

RecipeEngineWidget::RecipeEngineWidget(DispatchRecipeModule *dispatchModule, QWidget *parent) : QQuickWidget(parent)
{
    /*USERLEVEL lev = LoginData::instance()->getUserLevel();
    if (lev == USERLEVEL::OPERATOR)
    {
        operatorMode = true;
    }*/

    ui.setupUi(this);
    this->setSource(QUrl("qrc:/RecipeGui.qml"));
    this->setResizeMode(QQuickWidget::SizeRootObjectToView);
    this->rootContext()->setContextProperty("recipewidget", this);
    this->rootContext()->setContextProperty("_recipeDataModules", &m_modules);

    QObject *rootObject = this->rootObject();
    m_dispatchModule = dispatchModule;

    // signal from QML
    connect(rootObject, SIGNAL(executeModule(QString)), this, SLOT(initiateExecution(QString)));
    connect(rootObject, SIGNAL(startLoadingModules()), this, SLOT(onStartLoadingModules()));

    // signal from c++

    connect(m_dispatchModule, SIGNAL(dispatchResponseToRecipeGui(CORE::WrapperStatus, QString)), this,
            SLOT(onResponse(CORE::WrapperStatus, QString)));
    connect(Core::ICore::instance(), SIGNAL(emgStopBtnClicked()), this, SLOT(emgStopRecipe()));
    connect(&m_watcher, SIGNAL(finished()), this, SLOT(threadFinished()));

    if (operatorMode)
    {
        readFixRecipeFile();
    }

    // wait for response timer
    m_waitResTimer = new QTimer(this);
    connect(m_waitResTimer, &QTimer::timeout, this, &RecipeEngineWidget::onWaitResTimeOut);
}

RecipeEngineWidget::~RecipeEngineWidget()
{
    // future.cancel();
}

bool RecipeEngineWidget::getPause()
{
    return isPause;
}
void RecipeEngineWidget::setPause(bool val)
{
    if (isPause && !val)
    {
        isPause = val;
        waitObj.wakeOne();
    }
    isPause = val;
    RecipeWrapperRegistry::Instance()->notifyPauseStatus(val);
    ;
}
bool RecipeEngineWidget::getIsOperator()
{
    return operatorMode;
}
bool RecipeEngineWidget::getStatus()
{
    return m_IsRunning;
}

void RecipeEngineWidget::setStatus(bool s)
{
    m_IsRunning = s;
    RecipeWrapperRegistry::Instance()->notifyStopStatus(!s);
    emit statusChanged();
    if (isPause)
    {
        waitObj.wakeOne();
        isPause = false;
    }
}

QString RecipeEngineWidget::getFileName()
{
    return m_currentFile;
}

void RecipeEngineWidget::setFileName(QString newName)
{
    m_currentFile = newName;
    emit fileNameChanged();
}

bool RecipeEngineWidget::getMode()
{
    return m_mode;
}

void RecipeEngineWidget::setMode(bool newmode)
{
    m_mode = newmode;
}

void RecipeEngineWidget::delayAndExecute(int ms, QString msg)
{
    QThread::msleep(ms);
    this->onExecuteModule(msg);
}

QString RecipeEngineWidget::UrlToNativeFilePath(const QUrl &url) const
{
    return url.toLocalFile();
}

void RecipeEngineWidget::saveCurrentContextVar(QString value)
{
    Session::getInstance()->save(m_currentContextVar, value);
    m_currentContextVar = "";
}

void RecipeEngineWidget::readFile()
{
    QFile file(m_currentFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        // LoggingWrapper::getinstance()->debug("Could not open file");
        return;
    }
    // QTextStream in(&file);
    // while (!in.atEnd())
    //{
    //    QString line = in.readLine();
    //    line = line.trimmed();
    //    if (line != "" && line.at(0) != "#") // Todo remove all white spaces and trim
    //    {
    //        m_modules.addModule(line);
    //    }
    //}
    // file.close();
    QList<QString> list;
    QTextStream in(&file);
    int number = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        line = line.trimmed();

        Result ret = judgeLine(line, ++number);
        if(!ret.success){
            QMessageBox::warning(this, "Warning", QString::fromStdString(ret.errorMsg), QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        if (line != "" && line.at(0) != "#") // Todo remove all white spaces and trim
        {
            list.append(line);
        }
    }
    file.close();

    analysisForRecipeNew(list);
    saveResults("D:/");
}

#pragma region For loop
void RecipeEngineWidget::analysisForRecipe(QList<QString> list)
{
    QList<RecipeCycleInfo> cycleInfoList;
    int curIndex = -1;
    bool isJumpingOut = false;
    MetricsData::instance()->m_eyeboxQueue.clear();
    for (QString line : list)
    {
        if (line.contains("for") && line.contains("{"))
        {
            curIndex++;
            RecipeCycleInfo info;
            info.forIn = true;

            QStringList paralines = line.split("/");
            for (QString paraline : paralines)
            {
                int nameStart = paraline.indexOf("$");
                int nameEnd = paraline.indexOf("$", nameStart+1);
                QString name = paraline.mid(nameStart, nameEnd - nameStart + 1);

                int paraStart = paraline.indexOf("[");
                int paraEnd = paraline.indexOf("]");
                QString para = paraline.mid(paraStart + 1, paraEnd - paraStart - 1);
                info.paraListMap.insert(name, para.split(","));
                info.cycleCount = para.split(",").count();
                if (line.contains("$eyebox$"))
                {
                    MetricsData::instance()->setEyeboxQueue(para);
                }
            }

            cycleInfoList.append(info);

            continue;
        }
        else if (line == "}")
        {
            isJumpingOut = true;
            cycleInfoList[curIndex].forIn = false;
            curIndex--;
            if (curIndex == -1)
            {
                recursiveRecipe(cycleInfoList, 0, cycleInfoList.count());
            }
            continue;
        }
        else if (cycleInfoList.count() > 0 && curIndex != -1)
        {
            if (cycleInfoList[curIndex].forIn)
            {
                if (isJumpingOut)
                {
                    cycleInfoList[curIndex].recipeDownList.append(line);
                }
                else
                {
                    cycleInfoList[curIndex].recipeUpList.append(line);
                }
            }
        }
        else
        {
            if (condition(line))
            {
                m_modules.addModule(line);
            }
        }
    }
}

void RecipeEngineWidget::analysisForRecipeNew(QList<QString> list)
{
    m_forInfoMap.clear();
    m_forLineMap.clear();

    int curIndex = -1;
    MetricsData::instance()->m_eyeboxQueue.clear();
    for (QString line : list)
    {
        if (line.contains("for") && line.contains("{"))
        {
            curIndex++;

            QStringList paralines = line.split("/");
            for (QString paraline : paralines)
            {
                int nameStart = paraline.indexOf("$");
                int nameEnd = paraline.indexOf("$", nameStart + 1);
                QString name = paraline.mid(nameStart, nameEnd - nameStart + 1);

                int paraStart = paraline.indexOf("[");
                int paraEnd = paraline.indexOf("]");
                QString para = paraline.mid(paraStart + 1, paraEnd - paraStart - 1);
                m_forInfoMap[curIndex].insert(name, para.split(","));
                if (line.contains("$eyebox$"))
                {
                    MetricsData::instance()->setEyeboxQueue(para);
                }
            }
        }
        else if (line == "}")
        {
            forRecipe(curIndex);
            curIndex--;
        }
        else if (curIndex != -1)
        {
            m_forLineMap[curIndex].append(line);
        }
        else
        {
            if (condition(line))
            {
                m_modules.addModule(line);
            }
        }
    }
}

void RecipeEngineWidget::recursiveRecipe(QList<RecipeCycleInfo> cycleList, int floor, int count)
{
    if (floor == count)
    {
        return;
    }
    else
    {
        for (int cyc = 0; cyc < cycleList[floor].cycleCount; cyc++)
        {
            // Load outer parameter
            loadGlobalPara(cycleList, floor, cyc);
            insertRecipe(cycleList[floor].recipeUpList, cycleList[floor].paraListMap, cycleList[floor].globalParaMap,
                         cyc);
            recursiveRecipe(cycleList, floor + 1, count);
            insertRecipe(cycleList[floor].recipeDownList, cycleList[floor].paraListMap, cycleList[floor].globalParaMap,
                         cyc);
        }
    }
}
void RecipeEngineWidget::forRecipe(int curIndex)
{
    QMap<QString, QList<QString>> paramMap = m_forInfoMap[curIndex];
    int cycleCount = paramMap.begin().value().size();
    for (int i = 0; i < cycleCount; ++i)
    {
        for(QString str: m_forLineMap[curIndex]){
            if (str.contains("$"))
            {
                for (auto& key : paramMap.keys())
                {
                    if (str.contains(key) && !str.contains(key + "="))
                    {
                        str = str.replace(key, paramMap.value(key)[i]);
                    }
                }
            }

            if (curIndex == 0 && condition(str)) {
                m_modules.addModule(str);
            }else{
                m_forLineMap[curIndex-1].append(str);
            }
        }
    }
    m_forLineMap.remove(curIndex);
    m_forInfoMap.remove(curIndex);
}
void RecipeEngineWidget::loadGlobalPara(QList<RecipeCycleInfo> &cycleList, int floor, int cyc)
{
    for (int i = floor + 1; i < cycleList.count(); i++)
    {
        for (auto &key : cycleList[floor].paraListMap.keys())
        {
            cycleList[i].globalParaMap.insert(key, cycleList[floor].paraListMap.value(key)[cyc]);
        }
    }
}

bool RecipeEngineWidget::condition(const QString &line)
{
    //System:ConditionStart:/$eyebox$=5
    //System:ConditionEnd
    static bool flag = true;
    QStringList list = line.split(":");
    if (list.size() < 2){
        return flag;
    }
    if (list.at(0).toLower() != "system"){
        return flag;
    }
    
    if (list.at(1).trimmed().toLower() == QString("ConditionStart").toLower())
    {
        if (list.size() >= 3){
            QStringList tmpList = list.at(2).split("/");

            QString currentEyebox;
            QStringList filterList;
            for(QString paramstr: tmpList)
            {
                QStringList paramPair = paramstr.split("=");
                if (paramPair.size() == 2)
                {
                    QString key = paramPair[0].trimmed();
                    QString val = paramPair[1].trimmed();
                    if (key == "CurrentEyebox") {
                        currentEyebox = val;
                    }
                    else if (key == "Filter") {                
                        filterList = val.split(",");
                    }
                }
            }

            if (!filterList.contains(currentEyebox))
            {
                flag = false;
            }
        }
        return false;
    }
    else if (list.at(1).toLower() == QString("ConditionEnd").toLower())
    {
        flag = true;
        return false;
    }
    return flag;

    //TODO: for example, "reticle:reticleChange:/2222/condition=5(5,1)"
    list = line.split("/");
    if(list.size() == 0){
        return true;
    }

    for (QString str : list){
        QStringList strList =  str.split("=");
        if (strList.size() >= 2){
            if (strList.at(0).toLower() == "condition")
            {
                QStringList mmList = strList.at(1).split("(");
                if (mmList.size() >= 2)
                {
                    QString value = mmList.at(0);
                    QString tmp = mmList.at(1);
                    QStringList allValue = tmp.remove(")").split(",");
                    if (!allValue.contains(value)){
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

Result RecipeEngineWidget::judgeLine(const QString& line, int number)
{
    if (line.contains("[") && !line.contains("for")) {
        return Result(false, QString("Recipe file \"for\" error, line number is %1.").arg(number).toStdString());
    }
    return Result();
}

void RecipeEngineWidget::insertRecipe(QList<QString> recipeList, QMap<QString, QList<QString>> paraMap,
                                      QMap<QString, QString> globalParaMap, int cyc)
{
    for (QString str : recipeList)
    {
        if (str.contains("$"))
        {
            for (auto &key : paraMap.keys())
            {
                if (str.contains(key) && !str.contains(key + "="))
                {
                    str = str.replace(key, paraMap.value(key)[cyc]);
                }
            }
            for (auto &key : globalParaMap.keys())
            {
                if (str.contains(key))
                {
                    str = str.replace(key, globalParaMap.value(key));
                }
            }
        }

        if(condition(str)){
            m_modules.addModule(str);
        }
    }
}
#pragma endregion

inline bool RecipeEngineWidget::parseString(int &index, const QString &msg, QStringList &fields, bool isParameter)
{
    int start = index, balance = 0;
    QString token;

    while (index < msg.size())
    {
        if (msg.at(index) == ':' && !isParameter)
        {
            break;
        }
        if (msg.at(index) == '$')
        {
            if (balance == 0)
            {
                token.append(msg.mid(start, index - start));
                start = index;
                balance += 1;
            }
            else
            {
                auto a = Session::getInstance()->get(msg.mid(start, index - start + 1));
                start = index + 1;
                token.append(a);
                balance -= 1;
            }
        }
        index++;
    }
    token.append(msg.mid(start, index - start));
    if (token != "" && balance == 0)
        fields << token;
    index++;
    return balance != 0 && !token.isEmpty();
}

void RecipeEngineWidget::updateContextVar(QString msg)
{
    QStringList msgLst = msg.split("%");
    if (msgLst.size() == 3)
    {
        saveCurrentContextVar(msgLst[2]);
    }
}

inline bool RecipeEngineWidget::parseModule(const QString &msg, QStringList &fields)
{
    Session::getInstance()->loadReservedWords(); // params
    int index = 0;
    if (msg.at(0) == '$') // Equation
    {
        index++;
        while (msg.at(index) != '$')
        {
            index++;
        }
        m_currentContextVar = msg.mid(0, index + 1); // save variable
        index += 2;                                  // skip equalsign

        Session::getInstance()->setLastKey(m_currentContextVar);
    }

    parseString(index, msg, fields); // model
    parseString(index, msg, fields); // command
    parseString(index, msg, fields, true);
    Session::getInstance()->loadReservedWords(); // params
    return true;
}

void RecipeEngineWidget::onExecuteModule(QString msg)
{
    if (m_IsRunning && isPause)
    {
        mutex.lock();
        waitObj.wait(&mutex);
        mutex.unlock();
    }
    if (!m_IsRunning)
    {
        return;
    }
    QStringList fields;
    parseModule(msg, fields);
    if (fields.size() == 1)
    {
        if (m_currentContextVar != "")
        {
            saveCurrentContextVar(fields[0]);
            onResponse(CORE::Wrapper_Done, "");
        }
        else
            onResponse(CORE::Wrapper_Error_fatal, "Invalid input format");
    }
    else if (fields.size() == 2)
    {
        m_dispatchModule->dispatch(fields[0], fields[1]); // model, cmd
    }
    else if (fields.size() == 3)
    {
        m_dispatchModule->dispatch(fields[0], fields[1], fields[2]); // model, cmd, params
    }
    else
    {
        onResponse(CORE::Wrapper_Error_fatal, "Invalid input format");
    }
}

void RecipeEngineWidget::onStartLoadingModules()
{
    /*  if (LoginData::instance()->getUserLevelName() == "Operator")
      {*/
    if (PLCController::instance()->GetInterlockState())
    {

        bool isClosed = PLCController::instance()->GetDoorState();
        if (!isClosed)
        {
            emit disableQml();
            QMessageBox::warning(this, "Warning", "The door has been opened, please close it before testing.",
                                 QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }
    }
    /*  }*/
    m_modules.clear();
    readFile();
    emit finishedLoadingModulesQml();
}

bool RecipeEngineWidget::saveResults(const QString &path)
{
    // TODO:
    return true;

    // saving recipeResults                                          //TODO update reciperesults module with saving
    auto recipeData = m_modules.getData();
    QString filename = path + "/recipeResult.csv";
    if (QFile::exists(filename))
    {
        QFile::remove(filename);
    }

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << "module"
               << ","
               << "result"
               << ","
               << "error" << Qt::endl;
        for (auto i : recipeData)
        {
            stream << i.name << "," << i.result << "," << i.error << Qt::endl;
        }
        file.close();
    }
    else
    {
        // LoggingWrapper::getinstance()->debug("Could not save recipeResults");
        return false;
    }

    // saving contextVariables
    auto recipeContext = Session::getInstance()->getData();
    filename = path + "/recipeContext.csv";
    if (QFile::exists(filename))
    {
        QFile::remove(filename);
    }

    file.setFileName(filename);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << "ContextVariables"
               << ","
               << "Values" << Qt::endl;
        for (auto i : recipeContext.keys())
        {
            stream << i << "," << recipeContext[i] << Qt::endl;
        }
        file.close();
    }
    else
    {
        // LoggingWrapper::getinstance()->debug("Could not save contextVariables");
        return false;
    }
    // LoggingWrapper::getinstance()->debug("Results saved");
    return true;
}

void RecipeEngineWidget::onResponse(CORE::WrapperStatus s, QString msg)
{
    m_waitResTimer->stop();
    QString str = "=====Recipe engine===== " + msg;
    if (m_IsRunning)
    {
        switch (s)
        {
        case CORE::Wrapper_Init:
            break;
        case CORE::Wrapper_Start:
            break;
        case CORE::Wrapper_Done:
            // LoggingWrapper::getinstance()->debug(str.toStdString());
            if (str.contains("%updateVar:%"))
            {
                updateContextVar(msg);
            }
            emit doneQml();
            break;
        case CORE::Wrapper_Warning:
            if (m_mode)
            {
                emit errorQml(false, msg);
            }
            else
            {
                emit userResponseQml("Warning.qml", msg, false);
            }
            break;
        case CORE::Wrapper_Error:
            // LoggingWrapper::getinstance()->debug(str.toStdString());
            if (m_mode)
            {
                emit userResponseQml("Error.qml", msg, false);
            }
            else
            {
                emit errorQml(false, msg);
            }
            break;
        case CORE::Wrapper_AutoAOI_Error:
            break;
        case CORE::Wrapper_Image_Invalid:
            break;
        case CORE::Wrapper_Session_Get_DUT_Info:
            emit userResponseQml("SessionDialogDut.qml", "Enter Serial Number:", !m_currentContextVar.isEmpty());
            break;
        case CORE::Wrapper_Session_start:
            emit userResponseQml("SessionDialogStart.qml", "Please mount Dut onto hexapod", false);
            break;
        case CORE::Wrapper_Session_stop:
            emit userResponseQml("SessionDialogStop.qml", "Please remove Dut from hexapod", false);
            break;
        // case CORE::Wrapper_Recipe_stop:
        //    break;
        case CORE::Wrapper_Error_fatal:
            if (m_mode)
            {
                emit userResponseQml("Fatal.qml", msg, false);
            }
            else
            {
                emit errorQml(true, msg);
            }
            break;
        case CORE::Wrapper_Session_save:
            bool success = saveResults(msg);
            if (!success)
            {
                if (m_mode)
                {
                    emit userResponseQml("Error.qml", "Could not save data", false);
                }
                else
                {
                    emit errorQml(false, "Could not save data");
                }
            }
            else
                emit doneQml();
            break;
        }
    }
}

void RecipeEngineWidget::initiateExecution(QString msg)
{
    if (!msg.startsWith("System:"))
    {
        m_waitResTimer->start(m_waitMsec);
    }

    m_threadStart = true;
    QFuture<void> future = QtConcurrent::run(this, &RecipeEngineWidget::delayAndExecute, m_moduleDelay, msg);
    m_watcher.setFuture(future);
}

void RecipeEngineWidget::emgStopRecipe()
{
    emit emgStop();
}

void RecipeEngineWidget::onWaitResTimeOut()
{
    CORE::WrapperStatus s = CORE::WrapperStatus::Wrapper_Error_fatal;
    QString msg = "Wait Response time out";
    qWarning() << msg;

    emit onResponse(s, msg);
}

void RecipeEngineWidget::threadFinished()
{
    m_threadStart = false;
    WaitWidget::instance()->stopAnimation();
}

void RecipeEngineWidget::readFixRecipeFile()
{
    QString fieeContent = LoginData::instance()->getRecipeFileContent();
    QStringList lst = fieeContent.split(QRegExp("[\r\n]"), Qt::SkipEmptyParts);
    for (QString line : lst)
    {
        line = line.trimmed();
        if (line != "" && line.at(0) != "#") // Todo remove all white spaces and trim
        {
            if (condition(line))
            {
                m_modules.addModule(line);
            }
        }
    }
    emit finishedLoadingModulesQml();
}

void RecipeEngineWidget::recipeStarted()
{ 
    qWarning() << "---- takt time ---- [Recipe] start";
    QDateTime startTime = QDateTime::currentDateTime();
    QString current_str = startTime.toString("yyyyMMdd_hh:mm:ss");
    LoggingWrapper::instance()->debug(QString("[%1]Recipe Started").arg(current_str));
    MetricsData::instance()->setStartEyeboxTime(startTime);
    emit Core::PrjCommon::instance()->recipeRunStartOrEnd(true);
    canEndSignal = true;
}
void RecipeEngineWidget::recipeEnded()
{ 
    QDateTime endTime = QDateTime::currentDateTime();
    int secs = MetricsData::instance()->getStartEyeboxTime().secsTo(endTime);
    QString current_str = endTime.toString("yyyyMMdd_hh:mm:ss");
    LoggingWrapper::instance()->debug(QString("[%1]Recipe Ended----Recipe running totalTime is:%2 seconds").arg(current_str).arg(QString::number(secs)));
    qWarning() << "---- takt time ---- [Recipe] end";
    emit Core::PrjCommon::instance()->recipeRunStartOrEnd(false);
}
void RecipeEngineWidget::recipeStopped()
{ /*LoggingWrapper::getinstance()->debug("Recipe Stopped");*/
    qWarning() << "---- takt time ---- [Recipe] stop";
    emit Core::PrjCommon::instance()->recipeRunStartOrEnd(false);
    if (m_threadStart)
    {
        WaitWidget::instance()->startAnimation();
    }
}

void RecipeEngineWidget::recipeClapEnded()
{
    if (canEndSignal)
    {
        //emit userResponseQml("SessionDialogStop.qml", "Recipe done.", false);
        QMessageBox msg = QMessageBox();
        msg.setText(QString::fromStdString("Recipe done."));
        msg.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        msg.setStyleSheet("QLabel{"
                             "min-width: 200px;"
                             "min-height: 50px; "
                             "}");
        msg.exec();
        canEndSignal = false;
    }
}