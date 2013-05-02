
#pragma once

#include "SmartComponentBase.h"

#include <map>
#include <set>
#include <sstream>

class CommonBase;

class ComponentManager {
public:
	~ComponentManager() {};

	static ComponentManager& getSingleton(void) {
		static ComponentManager singleton;
		return singleton;
	}

	template <class Object>
	void registerObject(Object* object) {
		size_t type = typeid(object).hash_code();

		if (typeIsRegistered(type)) {
			return;
		}

		mObjects[object->metadata().name] = object;
		mRegistered.insert(type);
	}

	const wchar_t* getClassNames();

	CommonBase* createObject(std::wstring className);

	bool typeIsRegistered(size_t);

private:        
	std::map<std::wstring, CommonBase*> mObjects;
	std::set<size_t> mRegistered;

	ComponentManager() {};
	ComponentManager(const ComponentManager& root);
	ComponentManager& operator=(const ComponentManager&);
};

