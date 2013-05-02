
#include "ComponentManager.h"

long GetClassObject(const WCHAR_T* wsName, BaseNativeAPI::IComponentBase** pInterface) {
    if(*pInterface) return 0;

	try {
		*pInterface = ComponentManager::getSingleton().createObject(std::wstring(wsName));
	} catch (...) {
		return 0;
	}

    return (long)*pInterface;
}

long DestroyObject(BaseNativeAPI::IComponentBase** pIntf) {
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}

const WCHAR_T* GetClassNames() {
	return ComponentManager::getSingleton().getClassNames();
}

