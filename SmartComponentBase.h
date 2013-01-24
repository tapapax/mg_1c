
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

class Undefined {};

class BinaryData {
public:
	BinaryData(string data) : mData(data) {}

	const string& getData() const { return mData;}
private:
	string mData;
};

class SmartVariant {
	friend class SmartComponentBase;

public:
	class BadCast : public std::exception {
		virtual const char* what() const throw() {
			return "<wrong value type>";
		}
	};

	SmartVariant() : mContent(NULL) {}
	~SmartVariant() { delete mContent; }

	#define ADD_SUPPORT_TYPE(type) \
		SmartVariant(const type& value) : mContent(new ConcreteContent<type>(value)) {} \
		SmartVariant& operator=(const type& value) { SmartVariant(value).swap(*this); return *this; } \
		operator type() { return getValue<type>(); }

	ADD_SUPPORT_TYPE(string)
	ADD_SUPPORT_TYPE(wstring)
	ADD_SUPPORT_TYPE(long)
	ADD_SUPPORT_TYPE(double)
	ADD_SUPPORT_TYPE(bool)
	ADD_SUPPORT_TYPE(BinaryData)
	ADD_SUPPORT_TYPE(Undefined)
	#undef ADD_SUPPORT_TYPE

	SmartVariant(const SmartVariant& sec) : mContent(sec.mContent ? sec.mContent->clone() : 0) {}

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
	T getValue() {
		if (typeid(T) != mContent->type()) throw BadCast();
		return (static_cast<SmartVariant::ConcreteContent<T>*>(mContent))->mValue;
	}

	Content* mContent;
};

class SmartComponentBase : public IComponentBase {
public:
	typedef function<SmartVariant(SmartVariant*)> componentMethod;

private:
	class OKReturn {};

	wstring mLastErrorDescription;

	void putValueInVariant(const wstring& str, tVariant*);
	void putValueInVariant(const string& str, tVariant*);
	void putValueInVariant(const BinaryData& blob, tVariant*);
	void putValueInVariant(const bool value, tVariant*);
	void putValueInVariant(const long value, tVariant*);
	void putValueInVariant(const double value, tVariant*);

	const wstring mComponentName;

	SmartVariant extractVariant(tVariant* var);
	void packVariant(SmartVariant& svar, tVariant* var);

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

	SmartVariant getErrorDescription(SmartVariant*);

	void message(wstring msg, long code = 0);
};

#endif
