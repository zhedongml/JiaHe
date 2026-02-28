#include <unordered_map>
#include <mutex>
#include <string>
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

class OBJECTMANAGER_API ObjectManager2 {
public:
	static ObjectManager2& instance() {
		static ObjectManager2 inst;
		ObjectManager2* obj = &inst;
		return inst;
	}

	// 注册对象（任意类型）
	template <typename T>
	void registerObject(const std::string& id, T* obj) {
		std::lock_guard<std::mutex> lock(mutex_);
		object_map_[id] = obj;
	}

	// 获取对象（需要指定类型）
	template <typename T>
	T* getObject(const std::string& id) {
		std::lock_guard<std::mutex> lock(mutex_);
		auto it = object_map_.find(id);
		if (it == object_map_.end()) return nullptr;

		try {
			return std::any_cast<T*>(it->second);
		}
		catch (const std::bad_any_cast&) {
			throw std::runtime_error("Type mismatch when retrieving object for id: " + id);
		}
	}

	// 删除对象
	void unregisterObject(const std::string& id) {
		std::lock_guard<std::mutex> lock(mutex_);
		object_map_.erase(id);
	}

private:
	ObjectManager2() = default;

	std::unordered_map<std::string, std::any> object_map_;
	std::mutex mutex_;
};
