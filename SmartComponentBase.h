
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

class SmartComponentBase;

class SmartVariant {
	friend class SmartComponentBase;

public:
	class BadCast {};

	SmartVariant() : mContent(NULL) {}
	~SmartVariant() { delete mContent; }

	template <class T>
	SmartVariant(T& value) : mContent(new ConcreteContent<T>(value)) {}

	SmartVariant(const SmartVariant& sec) : mContent(sec.mContent ? sec.mContent->clone() : 0) {}

	template <typename T>
	SmartVariant& operator=(const T& sec) {
		SmartVariant(sec).swap(*this);
		return *this;
	}

	SmartVariant& operator=(const SmartVariant& sec) {
		SmartVariant(sec).swap(*this);
		return *this;
	}

	bool empty() const {
		return !mContent;
	}

	const std::type_info& type() const {
		return mContent ? mContent->type() : typeid(void);
	}

	SmartVariant& swap(SmartVariant& sec) {
		std::swap(mContent, sec.mContent);
		return *this;
	}

private:
	class Content {
	public:
		virtual ~Content() {}

		virtual const std::type_info& type() const = 0;
		virtual Content* clone() const = 0;
	};

	template <class T>
	class ConcreteContent: public Content {
	public: 
		ConcreteContent(const T& value): mValue(value) {}

		virtual const std::type_info& type() const { return typeid(T); }

		virtual ConcreteContent* clone() const {
			return new ConcreteContent(mValue);
		}

		T mValue;
	};

	template <class T>
	T* getValue() {
		if (typeid(T) != mContent->type()) throw BadCast();
		return &(static_cast<SmartVariant::ConcreteContent<T>*>(mContent))->mValue;
	}

	Content* mContent;
};

class BlobData {
public:
	BlobData(string data) : mData(data) {}

	const string& getData() const { return mData;}
private:
	string mData;
};

class SmartComponentBase : public IComponentBase {
public:
	typedef function<SmartVariant()> componentMethod;

private:
	class OKReturn {};

	tVariant* mReturnValueHandler;
	tVariant* mParametersHandler;
	long mParametersHandlerSize;
	wstring mLastErrorDescription;

	void packToVariant(const wstring& str, tVariant*);
	void packToVariant(const string& str, tVariant*);
	void packToVariant(const BlobData& blob, tVariant*);
	void packToVariant(const bool value, tVariant*);
	void packToVariant(const long value, tVariant*);
	void packToVariant(const double value, tVariant*);

	template <class Type> Type extractFromVariant(tVariant* var);
	template <>	wstring extractFromVariant<wstring>(tVariant* var);

	static wstring varToString(tVariant*);

	const wstring mComponentName;

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

	//void addProperty(wstring englishName, wstring localName, int modes, bool (Derived::*getter)(tVariant*), bool (Derived::*setter)(tVariant*));
	void addMethod(wstring englishName, wstring localName, long parametersCount, componentMethod method);

	template <class RetValType>	void returnValue(RetValType);
	template <class ParameterType> ParameterType getParameter(long number);

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

	SmartVariant getErrorDescription();

	void message(wstring msg, long code = 0);
};

template <class RetValType>
void SmartComponentBase::returnValue(RetValType value) {
	if (!mReturnValueHandler) return;

	packToVariant(value, mReturnValueHandler);

	throw OKReturn();
}

template <class ParameterType>
ParameterType SmartComponentBase::getParameter(long number) {
	if (mParametersHandler == NULL || number > mParametersHandlerSize || number < 1) return ParameterType();

	return extractFromVariant<ParameterType>(mParametersHandler + number - 1);
}

template <>
wstring SmartComponentBase::extractFromVariant<wstring>(tVariant* var) {
	return varToString(var);
}

#endif
