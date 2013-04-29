
#include "SmartComponentBase.h"

SmartVariant SmartComponentBase::getErrorDescription(SmartParameters) {
	return mLastErrorDescription;
}

SmartComponentBase::SmartComponentBase(wstring name)
	:
	mMemoryManager(NULL),
	mConnect(NULL),
	mComponentName(name)
{
	addMethod(L"ErrorDescription", L"ќписаниеќшибки", 0, bind(&SmartComponentBase::getErrorDescription, this, std::placeholders::_1));
}

SmartComponentBase::~SmartComponentBase() {

}

bool SmartComponentBase::Init(void* pConnection) { 
	mConnect = (IAddInDefBase*)pConnection;
	return mConnect != NULL;
}

long SmartComponentBase::GetInfo() { 
	return 2000; 
}

void SmartComponentBase::Done() {

}

bool SmartComponentBase::RegisterExtensionAs(WCHAR_T** wsExtensionName) { 
	if (!mMemoryManager) return false;
	if (!mMemoryManager->AllocMemory((void**)wsExtensionName, (mComponentName.size() + 1) * sizeof(WCHAR_T))) return false;

	lstrcpyW(*wsExtensionName, mComponentName.c_str());

	return true; 
}

long SmartComponentBase::GetNProps() { 
	return mProperties.size();
}

long SmartComponentBase::FindProp(const WCHAR_T* wsPropName) { 
	for (unsigned long i = 0; i < mProperties.size(); i++) {
		if (_wcsicmp(mProperties[i].englishName.c_str(), wsPropName) == 0 ||
			_wcsicmp(mProperties[i].localName.c_str(), wsPropName) == 0) return i;
	}

	return -1;
}

const WCHAR_T* SmartComponentBase::GetPropName(long lPropNum, long lPropAlias) { 
	if ((unsigned long)lPropNum >= mProperties.size()) return NULL;

	wstring* name;

	if (lPropAlias == 0)
		name = &mProperties[lPropNum].englishName;
	else
		name = &mProperties[lPropNum].localName;

	if (!mMemoryManager) return NULL;

	wchar_t* result;
	if (!mMemoryManager->AllocMemory((void**)&result, (name->size() + 1) * sizeof(WCHAR_T))) return NULL;

	lstrcpyW(result, name->c_str());

	return result;
}

bool SmartComponentBase::GetPropVal(const long lPropNum, tVariant* pvarPropVal) { 
	if ((unsigned long)lPropNum >= mProperties.size()) return false;

	auto property = mProperties[lPropNum];

	return false;
	//return ((Derived*)this->*property.getter)(pvarPropVal);
}

bool SmartComponentBase::SetPropVal(const long lPropNum, tVariant *varPropVal) { 
	if ((unsigned long)lPropNum >= mProperties.size()) return false;

	auto property = mProperties[lPropNum];

	return false;
	//return ((Derived*)this->*property.setter)(varPropVal);
}

bool SmartComponentBase::IsPropReadable(const long lPropNum) { 
	if ((unsigned long)lPropNum >= mProperties.size()) return false;

	return mProperties[lPropNum].modes & PROP_READABLE;
}

bool SmartComponentBase::IsPropWritable(const long lPropNum) {
	if ((unsigned long)lPropNum >= mProperties.size()) return false;

	return mProperties[lPropNum].modes & PROP_WRITEABLE;
}

long SmartComponentBase::GetNMethods() { 
	return mMethods.size();
}

long SmartComponentBase::FindMethod(const WCHAR_T* wsMethodName) { 
	for (unsigned long i = 0; i < mMethods.size(); i++) {
		if (_wcsicmp(mMethods[i].englishName.c_str(), wsMethodName) == 0 ||
			_wcsicmp(mMethods[i].localName.c_str(), wsMethodName) == 0) return i;
	}

	return -1;
}

const WCHAR_T* SmartComponentBase::GetMethodName(const long lMethodNum, const long lMethodAlias) { 
	if ((unsigned long)lMethodNum >= mMethods.size()) return NULL;

	wstring* name;

	if (lMethodAlias == 0)
		name = &mMethods[lMethodNum].englishName;
	else
		name = &mMethods[lMethodNum].localName;

	if (!mMemoryManager) return NULL;

	wchar_t* result;
	if (!mMemoryManager->AllocMemory((void**)&result, (name->size() + 1) * sizeof(WCHAR_T))) return NULL;

	lstrcpyW(result, name->c_str());

	return result;
}

long SmartComponentBase::GetNParams(const long lMethodNum) { 
	if ((unsigned long)lMethodNum >= mMethods.size()) return 0;

	return mMethods[lMethodNum].parametersCount;
}

bool SmartComponentBase::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue) { 
	TV_VT(pvarParamDefValue)= VTYPE_EMPTY;
	return false;
} 

bool SmartComponentBase::HasRetVal(const long lMethodNum) { 
	if ((unsigned long)lMethodNum >= mMethods.size()) return false;

	return true;
}

bool SmartComponentBase::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) { 
	return false;
}

bool SmartComponentBase::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) { 
	if ((unsigned long)lMethodNum >= mMethods.size()) return false;
	if (lSizeArray != mMethods[lMethodNum].parametersCount) return false;

	bool error = true;

	try {

		vector<SmartVariant> smartParameters(lSizeArray);
		for (int i = 0; i < lSizeArray; i++) {
			smartParameters[i] = extractVariant(paParams + i);
		}

		SmartVariant result = mMethods[lMethodNum].method(smartParameters);
		
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

void SmartComponentBase::SetLocale(const WCHAR_T* loc) {
	_wsetlocale(LC_ALL, loc);
}

bool SmartComponentBase::setMemManager(void* mem) {
	mMemoryManager = (IMemoryManager*)mem;
	return mMemoryManager != NULL;
}

//void CSmartComponentBase::addProperty(wstring englishName, wstring localName, int modes, bool (Derived::*getter)(tVariant*),
//									  bool (Derived::*setter)(tVariant*)) {
//	Property prop = { englishName, localName, modes, getter, setter };
//	mProperties.push_back(prop);
//}

void SmartComponentBase::addMethod(wstring englishName, wstring localName, long parametersCount, componentMethod method) {
	Method meth = { englishName, localName, parametersCount, method };
	mMethods.push_back(meth);
}

void SmartComponentBase::message(wstring msg, long code /*= 0*/) {
	mConnect->AddError(ADDIN_E_INFO, mComponentName.c_str(), msg.c_str(), code);
}

