
#ifndef _SMART_COMPONENT_BASE_H_
#define _SMART_COMPONENT_BASE_H_

#include <WTypes.h>
#include <string>
#include <vector>
#include <functional>

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

using namespace std;

class CBlob {
public:
	CBlob(string data) : mData(data) {}

	const string& getData() const { return mData;}
private:
	string mData;
};

class CSmartComponentBase : public IComponentBase {
public:
#define componentMethod function<void()>
	//	typedef function<void()> componentMethod;

private:
	class OKReturn {};

	tVariant* mReturnValueHandler;
	tVariant* mParametersHandler;
	long mParametersHandlerSize;
	wstring mLastErrorDescription;

	void packToVariant(const wstring& str, tVariant*);
	void packToVariant(const string& str, tVariant*);
	void packToVariant(const CBlob& blob, tVariant*);
	void packToVariant(bool value, tVariant*);
	void packToVariant(int32_t value, tVariant*);
	void packToVariant(double value, tVariant*);

	template <class Type> Type extractFromVariant(tVariant* var);
	template <>	wstring extractFromVariant<wstring>(tVariant* var);

	static wstring varToString(tVariant*);

protected:
	static const int PROP_READABLE = 1;
	static const int PROP_WRITEABLE = 2;

	struct Property {
		wstring englishName;
		wstring localName;
		int modes;
		//bool (Derived::*getter)(tVariant*);
		//bool (Derived::*setter)(tVariant*);
	};
	vector<Property> mProperties;

	struct Method {
		wstring englishName;
		wstring localName;
		long parametersCount;
		componentMethod method;
	};
	vector<Method> mMethods;

    IAddInDefBase* mConnect;
    IMemoryManager* mMemoryManager;

	wstring mComponentName;

	//void addProperty(wstring englishName, wstring localName, int modes, bool (Derived::*getter)(tVariant*), bool (Derived::*setter)(tVariant*));
	void addMethod(wstring englishName, wstring localName, long parametersCount, componentMethod method);

	template <class RetValType>	void returnValue(RetValType);
	template <class ParameterType> ParameterType getParameter(long number);

public:
	CSmartComponentBase();
	~CSmartComponentBase();

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

	void getErrorDescription();

	void viewError(wstring msg, long code = 237) {
		mConnect->AddError(ADDIN_E_FAIL, mComponentName.c_str(), msg.c_str(), code);
	}
};

template <class RetValType>
void CSmartComponentBase::returnValue(RetValType value) {
	if (!mReturnValueHandler) return;

	packToVariant(value, mReturnValueHandler);

	throw OKReturn();
}

template <class ParameterType>
ParameterType CSmartComponentBase::getParameter(long number) {
	if (mParametersHandler == NULL || number > mParametersHandlerSize || number < 1) return ParameterType();

	return extractFromVariant<ParameterType>(mParametersHandler + number - 1);
}

template <>
wstring CSmartComponentBase::extractFromVariant<wstring>(tVariant* var) {
	return varToString(var);
}

#endif
