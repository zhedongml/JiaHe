#include "pch.h"
#include "PyHelper.h"
#include <sstream>
#include "LogPlus.h"

void PyHelper::addPythonPathToPath(std::string directory)
{
    auto sysm = py::module_::import("sys");
    sysm.attr("path").attr("append")(directory);
}

void PyHelper::startPython()
{
    try {
        py::initialize_interpreter();
    }
    catch (std::exception e) {
        LOG4CPLUS_ERROR(LogPlus::getInstance()->logger, "Python interpreter initialize failed.");
    }
}

void PyHelper::endPython()
{
    py::finalize_interpreter();
}

std::vector<std::string> PyHelper::SplitStringByNewline(const std::string& str)
{
	auto result = std::vector<std::string>{};
	auto ss = std::stringstream{ str };

	for (std::string line; std::getline(ss, line, '\n');)
		result.push_back(line);

	return result;
}

std::string PyHelper::join(std::vector<std::string> const& strings, std::string delim)
{
    std::stringstream ss;
    std::copy(strings.begin(), strings.end(),
        std::ostream_iterator<std::string>(ss, delim.c_str()));
    return ss.str();
}