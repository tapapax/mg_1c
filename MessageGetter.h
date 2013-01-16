#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#include "stdafx.h"

#include "SmartComponentBase.h"

class CMessageGetter : public SmartComponentBase {
public:
	void getMessage();

	CMessageGetter() : SmartComponentBase(L"MessageGetter") {
		addMethod(L"GetMessage", L"ПолучитьПисьмо", 4, std::bind(&CMessageGetter::getMessage, this));
	}
};

#endif //__ADDINNATIVE_H__
