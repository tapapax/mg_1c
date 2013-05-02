
#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#include "stdafx.h"

#include "AddinObject.h"

using std::string;
using std::wstring;

class CMessageGetter : public Addin1C::AddinObject<CMessageGetter> {
public:
	Addin1C::Variant getMessage(Addin1C::VariantParameters);

	static Metadata getMetadata() {
		Metadata md(L"MessageGetter");
		md.addFunction(L"GetMessage", L"ПолучитьПисьмо", 4, &CMessageGetter::getMessage);
		return md;
	}
};

#endif //__ADDINNATIVE_H__
