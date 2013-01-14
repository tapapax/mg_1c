
#include "MessageGetter.h"

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if(!*pInterface) {
        *pInterface = new CMessageGetter;
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf) {
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames() {
    static WCHAR_T* names = L"Class1";
    return names;
}
//---------------------------------------------------------------------------//
