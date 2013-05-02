
#ifndef SmartVariant_h__
#define SmartVariant_h__

#include <string>
#include <exception>

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

#define ADD_TYPE_SUPPORT(type) \
	SmartVariant(const type& value) : mContent(new ConcreteContent<type>(value)) {} \
	SmartVariant& operator=(const type& value) { SmartVariant(value).swap(*this); return *this; } \
	operator type() { return getValue<type>(); }

class Undefined {};

class BinaryData {
public:
	BinaryData(std::string data) : mData(data) {}

	const std::string& getData() const { return mData;}
private:
	std::string mData;
};

class SmartVariant {
public:
	class BadCast : public std::exception {
		virtual const char* what() const throw() {
			return "<bad cast>";
		}
	};

	SmartVariant() : mContent(NULL) {}
	~SmartVariant() { delete mContent; }

	ADD_TYPE_SUPPORT(std::string)
	ADD_TYPE_SUPPORT(std::wstring)
	ADD_TYPE_SUPPORT(long)
	ADD_TYPE_SUPPORT(double)
	ADD_TYPE_SUPPORT(bool)
	ADD_TYPE_SUPPORT(BinaryData)
	ADD_TYPE_SUPPORT(Undefined)

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

	template <class T>
	T getValue() {
		if (typeid(T) != mContent->type()) throw BadCast();
		return (static_cast<SmartVariant::ConcreteContent<T>*>(mContent))->mValue;
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

	Content* mContent;
};

SmartVariant extractVariant(tVariant* var);
void packVariant(SmartVariant& svar, tVariant* var, IMemoryManager*);

#endif // SmartVariant_h__

