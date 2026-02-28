#pragma once
#include <iostream>
#include <map>

#ifdef _WIN32
#ifdef OBJECTMANAGER_EXPORTS
#define OBJECTMANAGER_API __declspec(dllexport)
#else
#define OBJECTMANAGER_API __declspec(dllimport)
#endif
#else
#define OBJECTMANAGER_API // Linux/Mac
#endif

class OBJECTMANAGER_API AsynObjectManger
{
public:
	static AsynObjectManger& instance() {
		static AsynObjectManger inst;
		return inst;
	}

	//Automatically set node information when registering asynchronous nodes
	void setNodeAsynInfo(int uid, void* treeNode);

	std::map<int, void*> getNodeAsynInfo();

	void ClearNodAsynInfo();

private:
	std::map<int, void*> nodeAsyn;
};

