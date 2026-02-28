#pragma once
#include <unordered_map>
#include <string>
#include <mutex>
#include <any>
#include <stdexcept>

#ifdef _WIN32
#ifdef OBJECTMANAGER_EXPORTS
#define OBJECTMANAGER_API __declspec(dllexport)
#else
#define OBJECTMANAGER_API __declspec(dllimport)
#endif
#else
#define OBJECTMANAGER_API // Linux/Mac
#endif

class OBJECTMANAGER_API ObjectManager {
public:
	static ObjectManager* getInstance();

	void registerObject(const std::string& id, const std::any& obj);
	void* getObject(const std::string& id) const;
	void unregisterObject(const std::string& id);

private:
	ObjectManager() = default;
	~ObjectManager() = default;

	ObjectManager(const ObjectManager&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;

	std::unordered_map<std::string, std::any> object_map_;
	mutable std::mutex mutex_;

	static ObjectManager* m_GlobalObjectManager;
};