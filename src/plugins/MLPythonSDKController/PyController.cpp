#include "PyController.h"
#include "LogPlus.h"
#include "PyHelper.h"
#include "PyStdErrOutStreamRedirect.h"
#include "pch.h"
#include <QCoreApplication>
#include "Core/icore.h"

QMutex PyController::mutex;
PyController *PyController::self = nullptr;
PyController::PyController(QObject *parent) : QObject(parent)
{
    
}

void PyController::InitPyController() {
    PyHelper::startPython();

    py::gil_scoped_release release;

    LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "Python interpreter successfully initialized.");

    InitPackage();

    bool res = connect(Core::ICore::instance(), &Core::ICore::coreAboutToClose, PyController::getInstance(), &PyController::endPython);
}
PyController *PyController::getInstance(QObject *parent)
{
    if (!self)
    {
        QMutexLocker locker(&mutex);
        self = new PyController();
    }
    self->setParent(parent);
    return self;
}

PyController::~PyController()
{
    PyHelper::endPython();
}

void PyController::endPython()
{
    delete iq_sdk;
    PyHelper::endPython();
}

void PyController::InitPackage()
{
    try
    {
        py::gil_scoped_acquire acquire;
        std::string setSysPath = QCoreApplication::applicationDirPath().toStdString() + "/pythonSDK";
        PyHelper::addPythonPathToPath(setSysPath);
        auto package = py::module_::import("PythonSDKCall");
        iq_sdk = new IQMetricsSDKCall(package);
    }
    catch (py::error_already_set e)
    {
        std::string error = std::string(e.what());
        handleError(e);
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, e.what());
    }
}

void PyController::calculateDistortion(string color, int eyeboxId, float degree)
{
    py::gil_scoped_release release;
    bool isSuccess = false;
    try
    {
        iq_sdk->DistortionCalculate(color, eyeboxId, degree);
        isSuccess = true;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "IQ metrics calculate by python successfully.");
    }
    catch (py::error_already_set &e)
    {
        handleError(e);
    }
    emit sigCalculateFinish(isSuccess);
}

void PyController::calculateFlare(string color, int eyeboxId, float degree)
{
    py::gil_scoped_release release;
    bool isSuccess = false;
    try
    {
        iq_sdk->FlareCalculate(color, eyeboxId, degree);
        isSuccess = true;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "IQ metrics calculate by python successfully.");
    }
    catch (py::error_already_set &e)
    {
        handleError(e);
    }
    emit sigCalculateFinish(isSuccess);
}

void PyController::calculateGhost(string color, int eyeboxId, float degree)
{
    py::gil_scoped_release release;
    bool isSuccess = false;
    try
    {
        iq_sdk->GhostCalculate(color, eyeboxId, degree);
        isSuccess = true;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "IQ metrics calculate by python successfully.");
    }
    catch (py::error_already_set &e)
    {
        handleError(e);
    }
    emit sigCalculateFinish(isSuccess);
}

void PyController::calculatePupilSwim(string color, int eyeboxId, float degree)
{
    py::gil_scoped_release release;
    bool isSuccess = false;
    try
    {
        iq_sdk->PupilSwimCalculate(color, eyeboxId, degree);
        isSuccess = true;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "IQ metrics calculate by python successfully.");
    }
    catch (py::error_already_set &e)
    {
        handleError(e);
    }
    emit sigCalculateFinish(isSuccess);
}

void PyController::calculateStrayLight(string color, int eyeboxId, float degree)
{
    py::gil_scoped_release release;
    bool isSuccess = false;
    try
    {
        iq_sdk->StrayLightCalculate(color, eyeboxId,degree);
        isSuccess = true;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "IQ metrics calculate by python successfully.");
    }
    catch (py::error_already_set &e)
    {
        handleError(e);
    }
    emit sigCalculateFinish(isSuccess);
}

void PyController::calculateColorUniformity(string color, int eyeboxId, float degree,string ndstr)
{
    py::gil_scoped_release release;
    bool isSuccess = false;
    try
    {
        iq_sdk->ColorUniformityCalculate(color, eyeboxId, degree,ndstr);
        isSuccess = true;
        LOG4CPLUS_INFO(LogPlus::getInstance()->logger, "IQ metrics calculate by python successfully.");
    }
    catch (py::error_already_set &e)
    {
        string aa=e.what();
        handleError(e);
    }
    emit sigCalculateFinish(isSuccess);
}

std::string PyController::printSysPath()
{
    PyStdErrOutStreamRedirect pyOutputRedirect{};

    auto sysm = py::module_::import("sys");

    //py::print(sysm.attr("path"));

    return pyOutputRedirect.stdoutString();
}

std::string PyController::getCurrDirectory()
{
    PyStdErrOutStreamRedirect pyOutputRedirect{};

    auto os = py::module::import("os");

    py::print(os.attr("getcwd")());

    std::cout << pyOutputRedirect.stdoutString() << std::endl;

    return pyOutputRedirect.stdoutString();
}

void PyController::handleError(py::error_already_set e)
{
    std::vector<std::string> errorStrings = PyHelper::SplitStringByNewline(e.what());
    std::string errorLine1 = errorStrings[0];
    LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, errorLine1.c_str());
}