
#include "ComponentManager.h"

long GetClassObject(const WCHAR_T* wsName, E1C_Component::BaseNativeAPI::IComponentBase** pInterface) {
    if(*pInterface) return 0;

	try {
		*pInterface = E1C_Component::ComponentManager::getSingleton().createObject(std::wstring(wsName));
	} catch (...) {
		return 0;
	}

    return (long)*pInterface;
}

long DestroyObject(E1C_Component::BaseNativeAPI::IComponentBase** pIntf) {
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}

const WCHAR_T* GetClassNames() {
	return E1C_Component::ComponentManager::getSingleton().getClassNames();
}

