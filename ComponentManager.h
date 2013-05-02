
#ifndef ComponentManager_h__
#define ComponentManager_h__

#include <map>
#include <set>

#include "BaseNativeAPI.h"

namespace E1C_Component {

	class AbstractComponentObject : public BaseNativeAPI::IComponentBase {
	public:
		virtual AbstractComponentObject* clone() = 0;
	};

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

			mObjects[object->metadata().name()] = object;
			mRegistered.insert(type);
		}

		const wchar_t* getClassNames();

		AbstractComponentObject* createObject(std::wstring className);

		bool typeIsRegistered(size_t);

	private:        
		std::map<std::wstring, AbstractComponentObject*> mObjects;
		std::set<size_t> mRegistered;

		ComponentManager() {};
		ComponentManager(const ComponentManager& root);
		ComponentManager& operator=(const ComponentManager&);
	};

}

#endif // ComponentManager_h__
