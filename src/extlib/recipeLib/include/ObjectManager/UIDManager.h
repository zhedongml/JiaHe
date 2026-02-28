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

class OBJECTMANAGER_API UIDManager
{
public:
	static UIDManager& instance() {
		static UIDManager inst;
		UIDManager* obj = &inst;
		return inst;
	}

	uint16_t getUID();

	void InitUID();

private:
	uint16_t uid__ = 1;
};

