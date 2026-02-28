#pragma once
#include <iostream>

#ifdef _WIN32
#ifdef OBJECTMANAGER_EXPORTS
#define OBJECTMANAGER_API __declspec(dllexport)
#else
#define OBJECTMANAGER_API __declspec(dllimport)
#endif
#else
#define OBJECTMANAGER_API // Linux/Mac
#endif

class OBJECTMANAGER_API TreePeriodManger
{
public:
	static TreePeriodManger& instance() {
		static TreePeriodManger inst;
		TreePeriodManger* obj = &inst;
		return inst;
	}

	void setTreeNodeDone(bool isDone);

	bool isTreeNodeDone();

private:
	bool isTreePeriodDone = true;
};

