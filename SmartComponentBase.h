
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
#include "ComponentManager.h"

#define CALL_MEMBER_FN(object,ptrToMember) ((object).*(ptrToMember))
		
using namespace std;

typedef std::vector<SmartVariant>& SmartParameters;

class CommonBase;

template <class Object>
class Metadata {
public:	
	typedef SmartVariant (Object::*ComponentFunction)(SmartParameters);
	typedef function<void(SmartVariant)> ComponentParameterSetter;
	typedef function<SmartVariant(void)> ComponentParameterGetter;

	Metadata(wstring _name)
		:
		name(_name)
	{}

	void addFunction(wstring englishName, wstring localName, long parametersCount, ComponentFunction method) {
		Function func = { englishName, localName, parametersCount, method };
		functions.push_back(func);
	}
	void addProperty(wstring englishName, wstring localName, ComponentParameterSetter setter, ComponentParameterGetter getter, int modes = PROP_READABLE | PROP_WRITEABLE) {
		Property prop = { englishName, localName, setter, getter, modes };
		properties.push_back(prop);
	}

//private:
	struct Function {
		wstring englishName;
		wstring localName;
		long parametersCount;
		ComponentFunction method;
	};
	struct Property {
		wstring englishName;
		wstring localName;
		int modes;
		ComponentParameterSetter setter;
		ComponentParameterGetter getter;
	};

	wstring name;

	vector<Function> functions;
	vector<Property> properties;
};

class CommonBase : public IComponentBase {
public:
	virtual CommonBase* clone() = 0;
};

template <class DerivedComponent>
class SmartComponentBase : public CommonBase {
private:
	wstring mComponentName;

	wstring mLastErrorDescription;
	IAddInDefBase* mConnect;
	IMemoryManager* mMemoryManager;

	static inline Metadata<DerivedComponent>& getMetadata() {
		static Metadata<DerivedComponent> md = DerivedComponent::getMetadata();
		md.addFunction(L"ErrorDescription", L"ќписаниеќшибки", 0, &DerivedComponent::getErrorDescription);
		return md;
	}

protected:
	static const int PROP_READABLE = 1;
	static const int PROP_WRITEABLE = 2;

	SmartComponentBase();

public:
	virtual ~SmartComponentBase() {}

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

	CommonBase* clone() {
		return new DerivedComponent;
	}

	void message(wstring msg, long code = 0);

	static inline Metadata<DerivedComponent>& metadata() {
		static Metadata<DerivedComponent>& md = getMetadata();
		return md;
	}
};


template <class DerivedComponent>
SmartVariant SmartComponentBase<DerivedComponent>::getErrorDescription(SmartParameters) {
	return mLastErrorDescription;
}

template <class DerivedComponent>
SmartComponentBase<DerivedComponent>::SmartComponentBase()
	:
	mMemoryManager(NULL),
	mConnect(NULL)
{
	mComponentName = metadata().name;
	
	ComponentManager::getSingleton().registerObject((DerivedComponent*)this);
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::Init(void* pConnection) { 
	mConnect = (IAddInDefBase*)pConnection;
	return mConnect != NULL;
}

template <class DerivedComponent>
long SmartComponentBase<DerivedComponent>::GetInfo() { 
	return 2000; 
}

template <class DerivedComponent>
void SmartComponentBase<DerivedComponent>::Done() {

}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::RegisterExtensionAs(WCHAR_T** wsExtensionName) { 
	if (!mMemoryManager) return false;
	if (!mMemoryManager->AllocMemory((void**)wsExtensionName, (mComponentName.size() + 1) * sizeof(WCHAR_T))) return false;

	lstrcpyW(*wsExtensionName, mComponentName.c_str());

	return true; 
}

template <class DerivedComponent>
long SmartComponentBase<DerivedComponent>::GetNProps() { 
	return metadata().properties.size();
}

template <class DerivedComponent>
long SmartComponentBase<DerivedComponent>::FindProp(const WCHAR_T* wsPropName) { 
	auto& md = metadata();
	
	for (auto i = 0; i < md.properties.size(); i++) {
		if (_wcsicmp(md.properties[i].englishName.c_str(), wsPropName) == 0 ||
			_wcsicmp(md.properties[i].localName.c_str(), wsPropName) == 0) return i;
	}

	return -1;
}

template <class DerivedComponent>
const WCHAR_T* SmartComponentBase<DerivedComponent>::GetPropName(long lPropNum, long lPropAlias) { 
	auto& props = metadata().properties;
	
	if ((unsigned long)lPropNum >= props.size()) return NULL;

	wstring* name;

	if (lPropAlias == 0)
		name = &props[lPropNum].englishName;
	else
		name = &props[lPropNum].localName;

	if (!mMemoryManager) return NULL;

	wchar_t* result;
	if (!mMemoryManager->AllocMemory((void**)&result, (name->size() + 1) * sizeof(WCHAR_T))) return NULL;

	lstrcpyW(result, name->c_str());

	return result;
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::GetPropVal(const long lPropNum, tVariant* pvarPropVal) { 
	auto& props = metadata().properties;

	if ((unsigned long)lPropNum >= props.size()) return false;

	auto property = props[lPropNum];

	bool error = true;

	try {

		packVariant(property.getter(), pvarPropVal, mMemoryManager);

		error = false;

	} catch (wstring& errorDescription) {
		mLastErrorDescription = errorDescription;
	} catch (std::exception& e) {
		std::string what = e.what();
		mLastErrorDescription = wstring(what.begin(), what.end());
	} catch (...) {
		mLastErrorDescription = L"<unknown error>";
	}

	if (!error) mLastErrorDescription.clear();

	return !error;
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::SetPropVal(const long lPropNum, tVariant *varPropVal) { 
	auto& props = metadata().properties;

	if ((unsigned long)lPropNum >= props.size()) return false;

	auto property = props[lPropNum];

	bool error = true;

	try {

		property.setter(extractVariant(varPropVal));

		error = false;

	} catch (wstring& errorDescription) {
		mLastErrorDescription = errorDescription;
	} catch (std::exception& e) {
		std::string what = e.what();
		mLastErrorDescription = wstring(what.begin(), what.end());
	} catch (...) {
		mLastErrorDescription = L"<unknown error>";
	}

	if (!error) mLastErrorDescription.clear();

	return !error;
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::IsPropReadable(const long lPropNum) { 
	auto& props = metadata().properties;

	if ((unsigned long)lPropNum >= props.size()) return false;

	return (props[lPropNum].modes & PROP_READABLE) != 0;
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::IsPropWritable(const long lPropNum) {
	auto& props = metadata().properties;

	if ((unsigned long)lPropNum >= props.size()) return false;

	return (props[lPropNum].modes & PROP_WRITEABLE) != 0;
}

template <class DerivedComponent>
long SmartComponentBase<DerivedComponent>::GetNMethods() { 
	return metadata().functions.size();
}

template <class DerivedComponent>
long SmartComponentBase<DerivedComponent>::FindMethod(const WCHAR_T* wsMethodName) { 
	auto& funcs = metadata().functions;

	for (auto i = 0; i < funcs.size(); i++) {
		if (_wcsicmp(funcs[i].englishName.c_str(), wsMethodName) == 0 ||
			_wcsicmp(funcs[i].localName.c_str(), wsMethodName) == 0) return i;
	}

	return -1;
}

template <class DerivedComponent>
const WCHAR_T* SmartComponentBase<DerivedComponent>::GetMethodName(const long lMethodNum, const long lMethodAlias) { 
	auto& funcs = metadata().functions;

	if ((unsigned long)lMethodNum >= funcs.size()) return NULL;

	wstring* name;

	if (lMethodAlias == 0)
		name = &funcs[lMethodNum].englishName;
	else
		name = &funcs[lMethodNum].localName;

	if (!mMemoryManager) return NULL;

	wchar_t* result;
	if (!mMemoryManager->AllocMemory((void**)&result, (name->size() + 1) * sizeof(WCHAR_T))) return NULL;

	lstrcpyW(result, name->c_str());

	return result;
}

template <class DerivedComponent>
long SmartComponentBase<DerivedComponent>::GetNParams(const long lMethodNum) { 
	auto& funcs = metadata().functions;

	if ((unsigned long)lMethodNum >= funcs.size()) return 0;

	return funcs[lMethodNum].parametersCount;
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue) { 
	TV_VT(pvarParamDefValue)= VTYPE_EMPTY;
	return false;
} 

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::HasRetVal(const long lMethodNum) { 
	if ((unsigned long)lMethodNum >= metadata().functions.size()) return false;

	return true;
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) { 
	return false;
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) { 
	auto& funcs = metadata().functions;

	if ((unsigned long)lMethodNum >= funcs.size()) return false;
	if (lSizeArray != funcs[lMethodNum].parametersCount) return false;

	bool error = true;

	try {

		vector<SmartVariant> smartParameters(lSizeArray);
		for (int i = 0; i < lSizeArray; i++) {
			smartParameters[i] = extractVariant(paParams + i);
		}

		//SmartVariant result = funcs[lMethodNum].method(smartParameters);
		SmartVariant result = CALL_MEMBER_FN(*((DerivedComponent*)this), funcs[lMethodNum].method)(smartParameters);
		
		for (int i = 0; i < lSizeArray; i++) {
			packVariant(smartParameters[i], paParams + i, mMemoryManager);
		}

		packVariant(result, pvarRetValue, mMemoryManager);

		error = false;

	} catch (wstring& errorDescription) {
		mLastErrorDescription = errorDescription;
	} catch (std::exception& e) {
		std::string what = e.what();
		mLastErrorDescription = wstring(what.begin(), what.end());
	} catch (...) {
		mLastErrorDescription = L"<unknown error>";
	}

	if (!error) mLastErrorDescription.clear();

	return !error;
}

template <class DerivedComponent>
void SmartComponentBase<DerivedComponent>::SetLocale(const WCHAR_T* loc) {
	_wsetlocale(LC_ALL, loc);
}

template <class DerivedComponent>
bool SmartComponentBase<DerivedComponent>::setMemManager(void* mem) {
	mMemoryManager = (IMemoryManager*)mem;
	return mMemoryManager != NULL;
}

template <class DerivedComponent>
void SmartComponentBase<DerivedComponent>::message(wstring msg, long code /*= 0*/) {
	mConnect->AddError(ADDIN_E_INFO, mComponentName.c_str(), msg.c_str(), code);
}

#endif
