#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

#define RELATIVE(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

static const void* game_rbx_jmp;

class FNTool
{
public:
	bool Initialize(const char* GObjects_Signature, const char* GetNameByIndex_Signature, const char* FnFree_Signature);
	uintptr_t FindSignature(const char* Signature);
	DWORD FindOffset(const char* Class, const char* var);
	VOID FindAndLogOffset(const char* Class, const char* var);
	VOID FindAndLogClass(const char* Class);

private:
	uintptr_t UObjectArray = NULL;
	uintptr_t GetNameByIndex = NULL;
	uintptr_t FnFree = NULL;
	DWORD ObjectsCount = 0;

	void cFixName(char* Name);
	void FreeObjName(uintptr_t Address);
	char* fGetObjectName(uintptr_t Object);
	char* fGetNameByIndex(int Index);
};