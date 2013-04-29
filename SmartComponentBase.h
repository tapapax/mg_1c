
#ifndef _SMART_COMPONENT_BASE_H_
#define _SMART_COMPONENT_BASE_H_

#include <WTypes.h>
#include <string>
#include <vector>
#include <functional>

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

#include "SmartVariant.h"

using namespace std;

class SmartComponentBase : public IComponentBase {
public:
	typedef std::vector<SmartVariant>& SmartParameters;
	typedef function<SmartVariant(SmartParameters)> ComponentFunction;
	typedef function<void(SmartVariant)> ComponentParameterSetter;
	typedef function<SmartVariant(void)> ComponentParameterGetter;

private:
	wstring mLastErrorDescription;
	IAddInDefBase* mConnect;
	IMemoryManager* mMemoryManager;

	const wstring mComponentName;

protected:
	static const int PROP_READABLE = 1;
	static const int PROP_WRITEABLE = 2;

	struct Property {
		wstring englishName;
		wstring localName;
		int modes;
		ComponentParameterSetter setter;
		ComponentParameterGetter getter;
	};
	vector<Property> mProperties;

	struct Method {
		wstring englishName;
		wstring localName;
		long parametersCount;
		ComponentFunction method;
	};
	vector<Method> mMethods;

	void addFunction(wstring englishName, wstring localName, long parametersCount, ComponentFunction method);
	void addProperty(wstring englishName, wstring localName, ComponentParameterSetter, ComponentParameterGetter, int modes = PROP_READABLE | PROP_WRITEABLE);
	//void addProperty(wstring englishName, wstring localName, , int modes = PROP_READABLE | PROP_WRITEABLE);

	//void trivialSetter();

public:
	SmartComponentBase(wstring name);
	~SmartComponentBase();

    virtual bool ADDIN_API Init(void*);
    virtual bool ADDIN_API setMemManager(void* mem);
    virtual long ADDIN_API GetInfo();
    virtual void ADDIN_API Done();
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T**);
    virtual long ADDIN_API GetNProps();
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal);
    virtual bool ADDIN_API IsPropReadable(const long lPropNum);
    virtual bool ADDIN_API IsPropWritable(const long lPropNum);
    virtual long ADDIN_API GetNMethods();
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, const long lMethodAlias);
    virtual long ADDIN_API GetNParams(const long lMethodNum);
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue);   
    virtual bool ADDIN_API HasRetVal(const long lMethodNum);
    virtual bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray);
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc);

	SmartVariant getErrorDescription(SmartParameters);

	void message(wstring msg, long code = 0);
};

#endif
