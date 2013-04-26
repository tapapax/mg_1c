#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#include "stdafx.h"

#include "SmartComponentBase.h"

class CMessageGetter : public SmartComponentBase {
public:
	SmartVariant getMessage(SmartParameters);

	CMessageGetter();
};

#endif //__ADDINNATIVE_H__
