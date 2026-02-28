#pragma once

#pragma push_macro("slots")
#undef slots
#include <pybind11\embed.h>
#pragma pop_macro("slots")

#include "mlpythonsdkcontroller_global.h"

namespace py = pybind11;

//#ifdef MLPYTHONSDKCONTROLLER_EXPORTS
//#define MLPYTHONSDKCONTROLLER_API __declspec(dllexport)
//#else
//#define MLPYTHONSDKCONTROLLER_API __declspec(dllimport)
//#endif

class MLPYTHONSDKCONTROLLER_EXPORT PyStdErrOutStreamRedirect
{
    py::object _stdout;
    py::object _stderr;
    py::object _stdout_buffer;
    py::object _stderr_buffer;
public:
    PyStdErrOutStreamRedirect() {
        auto sysm = py::module_::import("sys");
        _stdout = sysm.attr("stdout");
        _stderr = sysm.attr("stderr");
        auto stringio = py::module::import("io").attr("StringIO");
        _stdout_buffer = stringio();  // Other filelike object can be used here as well, such as objects created by pybind11
        _stderr_buffer = stringio();
        sysm.attr("stdout") = _stdout_buffer;
        sysm.attr("stderr") = _stderr_buffer;
    }
    std::string stdoutString() {
        _stdout_buffer.attr("seek")(0);
        return py::str(_stdout_buffer.attr("read")());
    }
    std::string stderrString() {
        _stderr_buffer.attr("seek")(0);
        return py::str(_stderr_buffer.attr("read")());
    }
    ~PyStdErrOutStreamRedirect() {
        auto sysm = py::module_::import("sys");
        sysm.attr("stdout") = _stdout;
        sysm.attr("stderr") = _stderr;
    }
};

