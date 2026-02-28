#pragma once

#pragma push_macro("slots")
#undef slots
#include <pybind11\embed.h>
#pragma pop_macro("slots")

namespace py = pybind11;

#ifdef MLPYTHONSDKCONTROLLER_EXPORTS
#define MLPYTHONSDKCONTROLLER_API __declspec(dllexport)
#else
#define MLPYTHONSDKCONTROLLER_API __declspec(dllimport)
#endif

class MLPYTHONSDKCONTROLLER_API PyHelper
{
public:
    static void addPythonPathToPath(std::string directory);

	static void startPython();
	static void endPython();

	static std::vector<std::string> SplitStringByNewline(const std::string& str);

	static std::string join(std::vector<std::string> const& strings, std::string delim);
};

