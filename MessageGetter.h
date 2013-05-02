
#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#include "stdafx.h"

#include "ComponentObject.h"

using std::string;
using std::wstring;

class CMessageGetter : public E1C_Component::ComponentObject<CMessageGetter> {
public:
	E1C_Component::Variant getMessage(E1C_Component::VariantParameters);

	static E1C_Component::Metadata<CMessageGetter> getMetadata() {
		E1C_Component::Metadata<CMessageGetter> md(L"MessageGetter");
		md.addFunction(L"GetMessage", L"ПолучитьПисьмо", 4, &CMessageGetter::getMessage);
		return md;
	}
};

#endif //__ADDINNATIVE_H__
