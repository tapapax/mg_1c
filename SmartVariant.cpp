
#include "SmartVariant.h"

SmartVariant extractVariant(tVariant* var) {
	SmartVariant result;

	if (var->vt == VTYPE_BOOL) result = var->bVal;
	else if (var->vt == VTYPE_I2 || var->vt == VTYPE_I4 || var->vt == VTYPE_ERROR || var->vt == VTYPE_UI1) result = (long)var->lVal;
	else if (var->vt == VTYPE_R4 || var->vt == VTYPE_R8 /*|| var->vt == VTYPE_CY*/) result = var->dblVal;
	else if (var->vt == VTYPE_PWSTR) result = std::wstring(var->pwstrVal, var->wstrLen);
	else if (var->vt == VTYPE_EMPTY) result = Undefined();
	else throw std::wstring(L"<unsupported variant type>");

	return result;
}

class BadReturnValue : public std::exception {
	virtual const char* what() const throw() {
		return "<bad return value>";
	}
};

void putDoubleInVariant(const double value, tVariant* var) {
	TV_VT(var) = VTYPE_R8;
	TV_R8(var) = value;
}

void putLongInVariant(const long value, tVariant* var) {
	TV_VT(var) = VTYPE_I4;
	TV_I4(var) = value;
}

void putBoolInVariant(const bool value, tVariant* var) {
	TV_VT(var) = VTYPE_BOOL;
	TV_BOOL(var) = value;
}

void putWStringInVariant(const std::wstring& str, tVariant* var, IMemoryManager* memoryManager) {
	wchar_t* ptr;
	auto size = (str.size() + 1) * sizeof(wchar_t);

	if (!memoryManager->AllocMemory((void**)&ptr, size)) {
		throw std::wstring(L"Allocation error");
	}

	memcpy(ptr, str.c_str(), size);

	TV_VT(var) = VTYPE_PWSTR;
	TV_WSTR(var) = ptr;
	var->wstrLen = str.size();
}

void putStringInVariant(const std::string& str, tVariant* var, IMemoryManager* memoryManager) {
	char* ptr;
	auto size = (str.size() + 1) * sizeof(char);

	if (!memoryManager->AllocMemory((void**)&ptr, size)) {
		throw std::wstring(L"Allocation error");
	}

	memcpy(ptr, str.c_str(), size);

	TV_VT(var) = VTYPE_PSTR;
	TV_STR(var) = ptr;
	var->strLen = str.size();
}

void putBinaryInVariant(const BinaryData& blob, tVariant* var, IMemoryManager* memoryManager){
	putStringInVariant(blob.getData(), var, memoryManager);
	TV_VT(var) = VTYPE_BLOB;
}

void packVariant(SmartVariant& svar, tVariant* var, IMemoryManager* memoryManager) {
	if (svar.type() == typeid(std::wstring))
		putWStringInVariant(svar.getValue<std::wstring>(), var, memoryManager);
	else if (svar.type() == typeid(std::string))
		putStringInVariant(svar.getValue<std::string>(), var, memoryManager);
	else if (svar.type() == typeid(double))
		putDoubleInVariant(svar.getValue<double>(), var);
	else if (svar.type() == typeid(bool))
		putBoolInVariant(svar.getValue<bool>(), var);
	else if (svar.type() == typeid(BinaryData))
		putBinaryInVariant(svar.getValue<BinaryData>(), var, memoryManager);
	else if (svar.type() == typeid(long))
		putLongInVariant(svar.getValue<long>(), var);
	else if (svar.type() == typeid(Undefined))
		var->vt = VTYPE_EMPTY;
	else throw BadReturnValue();
}

