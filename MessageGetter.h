
#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#include "stdafx.h"

#include "SmartComponentBase.h"

using namespace std;

class CMessageGetter : public SmartComponentBase<CMessageGetter> {
public:
	SmartVariant getMessage(SmartParameters);

	static Metadata<CMessageGetter> getMetadata() {
		Metadata<CMessageGetter> md(L"MessageGetter");
		md.addFunction(L"GetMessage", L"ПолучитьПисьмо", 4, &CMessageGetter::getMessage);
		return md;
	}
};

#endif //__ADDINNATIVE_H__
