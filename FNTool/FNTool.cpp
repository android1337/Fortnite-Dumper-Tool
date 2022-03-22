#include "FNTool.h"
#include "spoof_call.h"
#include "Memory.h"
#include <fstream>

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	int Count;
	int Max;
};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

struct FName
{
	int32_t ComparisonIndex;
	int32_t Number;
};

bool FNTool::Initialize(const char* GObjects_Signature, const char* GetNameByIndex_Signature, const char* FnFree_Signature)
{

	uintptr_t GObjects = (uintptr_t)MemoryHelper::Pattern::PatternScan(GObjects_Signature); if (!GObjects) MessageBoxA(0, "Failed To Find UObjects", "Error", MB_ICONERROR);
	GObjects = RELATIVE(GObjects, 7);
	uintptr_t GetNameByIndex = (uintptr_t)MemoryHelper::Pattern::PatternScan(GetNameByIndex_Signature); if (!GetNameByIndex) MessageBoxA(0, "Failed To Find GetNameByIndex", "Error", MB_ICONERROR);
	uintptr_t FnFree = (uintptr_t)MemoryHelper::Pattern::PatternScan(FnFree_Signature); if (!FnFree) MessageBoxA(0, "Failed To Find FnFree", "Error", MB_ICONERROR);
	FnFree = RELATIVE(FnFree, 5);

	if (!GObjects || !GetNameByIndex || !FnFree) return false;

	game_rbx_jmp = gadget(NULL);

	this->ObjectsCount = *(DWORD*)(GObjects + 0x14);
	if (this->ObjectsCount > 0x9000) this->ObjectsCount = 0x9000;

	uintptr_t GObjects_deref = *(uintptr_t*)GObjects;
	if (spoof_call(game_rbx_jmp, IsBadWritePtr, (const LPVOID)GObjects_deref, (UINT_PTR)8)) { MessageBoxA(0, "Failed To Initialize", "Error", MB_ICONERROR); return FALSE; }

	uintptr_t GObjects_deref2 = *(uintptr_t*)GObjects_deref;
	if (spoof_call(game_rbx_jmp, IsBadWritePtr, (const LPVOID)GObjects_deref2, (UINT_PTR)8)) { MessageBoxA(0, "Failed To Initialize", "Error", MB_ICONERROR); return FALSE; }

	this->UObjectArray = GObjects_deref2;
	this->GetNameByIndex = GetNameByIndex;
	this->FnFree = FnFree;

	return true;
}

BOOLEAN MaskCompare(PVOID buffer, LPCSTR pattern, LPCSTR mask) {
	for (auto b = reinterpret_cast<PBYTE>(buffer); *mask; ++pattern, ++mask, ++b) {
		if (*mask == 'x' && *reinterpret_cast<LPCBYTE>(pattern) != *b) {
			return FALSE;
		}
	}

	return TRUE;
}

PBYTE FindPattern2(LPCSTR pattern, LPCSTR mask) {
	MODULEINFO info = { 0 };
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

	info.SizeOfImage -= static_cast<DWORD>(strlen(mask));

	for (auto i = 0UL; i < info.SizeOfImage; ++i) {
		auto addr = reinterpret_cast<PBYTE>(info.lpBaseOfDll) + i;
		if (MaskCompare(addr, pattern, mask)) {
			return addr;
		}
	}
}

uintptr_t FNTool::FindSignature(const char* Signature) {
	uintptr_t SigResult = MemoryHelper::Pattern::PatternScan(Signature);
	return SigResult;
}

VOID FNTool::FindAndLogOffset(const char* Class, const char* OffsetName)
{
	int strlenght = strlen((std::string("") + Class + "." + OffsetName).c_str());

	int calculation = 52 - strlenght;

	std::string strspace = "";

	for (int i = 0; i < calculation; i++) {
		strspace = strspace + " ";
	}

	std::cout << "  " << Class << "." << OffsetName << strspace << " ->   0x" << std::hex << std::uppercase << this->FindOffset(Class, OffsetName) << std::endl;

}

DWORD FNTool::FindOffset(const char* Class, const char* varName)
{
	for (DWORD i = 0x0; i < this->ObjectsCount; i++)
	{
		auto CurrentObject = *(uintptr_t*)(this->UObjectArray + (i * 0x18));

		if (!CurrentObject) { MessageBoxA(0, "curobjnull", 0, 0); return NULL; }
		if (!(*(uintptr_t*)(CurrentObject + 0x50)) || *(DWORD*)(CurrentObject + 0x54) == 0xFFFFFFFF) continue;


		char* CurObjectName = this->fGetObjectName(CurrentObject);

		if (!strcmp(CurObjectName, Class)) //Same class
		{
			for (auto Property = *(uint64_t*)(CurrentObject + 0x50); !spoof_call(game_rbx_jmp, IsBadWritePtr, (LPVOID)Property, (UINT_PTR)8); Property = *(uint64_t*)(Property + 0x20))
			{
				auto Type = *(uint64_t*)(Property + 0x8);

				if (!spoof_call(game_rbx_jmp, IsBadWritePtr, (LPVOID)Type, (UINT_PTR)8) && Type)
				{
					auto Property_FName = *(FName*)(Property + 0x28);
					auto Offset = *(DWORD*)(Property + 0x44);

					if (Offset != 0)
					{
						auto Property_idx = Property_FName.ComparisonIndex;

						if (Property_idx)
						{
							char* PropertyName = this->fGetNameByIndex(Property_idx);

							if (!strcmp(PropertyName, varName))
							{
								return Offset;
							}
						}
					}
				}

			}

			return NULL; //Return NULL if checked all properties of the class.
		}
	}
}

VOID FNTool::FindAndLogClass(const char* Class)
{
	std::cout << "\n";
	for (DWORD i = 0x0; i < this->ObjectsCount; i++)
	{
		auto CurrentObject = *(uintptr_t*)(this->UObjectArray + (i * 0x18));

		if (!CurrentObject) return;
		if (!(*(uintptr_t*)(CurrentObject + 0x50)) || *(DWORD*)(CurrentObject + 0x54) == 0xFFFFFFFF) continue;


		char* CurObjectName = this->fGetObjectName(CurrentObject);

		if (!strcmp(CurObjectName, Class)) //Same class
		{
			for (auto Property = *(uint64_t*)(CurrentObject + 0x50); !IsBadWritePtr((LPVOID)Property, 8); Property = *(uint64_t*)(Property + 0x20))
			{
				auto Type = *(uint64_t*)(Property + 0x8);

				if (!IsBadWritePtr((LPVOID)Type, 8) && Type)
				{
					auto Property_FName = *(FName*)(Property + 0x28);
					auto Offset = *(DWORD*)(Property + 0x44);

					if (Offset != 0)
					{
						auto Property_idx = Property_FName.ComparisonIndex;

						if (Property_idx)
						{
							char* PropertyName = this->fGetNameByIndex(Property_idx);

							int strlenght = strlen((std::string("") + PropertyName).c_str());

							int calculation = 52 - strlenght;

							std::string strspace = "";

							for (int iii = 0; iii < calculation; iii++) {
								strspace = strspace + " ";
							}
							int jjjj = 0;

							for (int iiiii = 0; iiiii < strlenght; iiiii++)
								if (PropertyName[iiiii] >= 0 && PropertyName[iiiii] <= 127)
									PropertyName[jjjj++] = PropertyName[iiiii];

							std::cout << "  " << PropertyName << strspace << " ->   0x" << std::hex << std::uppercase << Offset << std::endl;

						}
					}
				}

			}

			return;
		}

	}
	std::cout << "  [!] Class doesn't exists\n";
}

void FNTool::cFixName(char* Name)
{
	for (int i = 0; Name[i] != '\0'; i++)
	{
		if (Name[i] == '_')
		{
			if (Name[i + 1] == '0' ||
				Name[i + 1] == '1' ||
				Name[i + 1] == '2' ||
				Name[i + 1] == '3' ||
				Name[i + 1] == '4' ||
				Name[i + 1] == '5' ||
				Name[i + 1] == '6' ||
				Name[i + 1] == '7' ||
				Name[i + 1] == '8' ||
				Name[i + 1] == '9')
				Name[i] = '\0';
		}
	}

	return;
}

void FNTool::FreeObjName(uintptr_t Address)
{
	if (Address == NULL) return;

	auto func = reinterpret_cast<__int64(__fastcall*)(__int64)>(this->FnFree);

	spoof_call(game_rbx_jmp, func, (__int64)Address);
}

char* FNTool::fGetObjectName(uintptr_t Object)
{
	if (Object == 0) return (char*)"";

	char* return_string = FNTool::fGetNameByIndex(*(int*)(Object + 0x18));

	if (return_string == NULL) return (char*)"";

	return return_string;
}

char* FNTool::fGetNameByIndex(int Index)
{
	if (Index == 0) return (char*)"";

	auto fGetNameByIdx = reinterpret_cast<FString * (__fastcall*)(int*, FString*)>(this->GetNameByIndex);

	FString result;
	spoof_call(game_rbx_jmp, fGetNameByIdx, &Index, &result);

	if (result.c_str() == NULL) return (char*)"";

	auto tmp = result.ToString();

	char return_string[1024];
	spoof_call(game_rbx_jmp, memcpy, (void*)return_string, (const void*)std::string(tmp.begin(), tmp.end()).c_str(), (size_t)1024);

	FreeObjName((uintptr_t)result.c_str());

	cFixName(return_string);

	return return_string;
}
