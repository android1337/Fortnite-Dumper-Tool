#include <Windows.h>
#include <iostream>
#include <thread>
#include <process.h>
#include "FNTool.h"


VOID FindCustomOffset(FNTool* FN) {
	std::cout << "  Input the class name :  ";
	std::string inputclass;
	std::cin >> inputclass;
	std::cout << "\n";
	if (inputclass != "") {
		std::cout << "  Input the offset name :  ";
		std::string inputoffset;
		std::cin >> inputoffset;
		std::cout << "\n";
		if (inputoffset != "") {
			if (FN->FindOffset(inputclass.c_str(), inputoffset.c_str()) != 0) { FN->FindAndLogOffset(inputclass.c_str(), inputoffset.c_str()); std::cout << "  [+] Offset Succesfully Found\n"; }
			else {
				std::cout << "  [!] Failed\n"; return;
			}


		}
		else {
			std::cout << "  [!] Failed\n";
			return;
		}
	}
	else {
		std::cout << "  [!] Failed\n";
		return;
	}
}

VOID FindCustomClass(FNTool* FN) {
	std::cout << "  Input the class name :  ";
	std::string inputclass;
	std::cin >> inputclass;
	std::cout << "\n";
	if (inputclass != "") {
		FN->FindAndLogClass(inputclass.c_str());
		std::cout << "  [+] Class Succesfully Dumped\n";
	}
	else {
		std::cout << "  [!] Failed\n";
		return;
	}
}

VOID FindMainOffsets(FNTool* FN) {

	//Get the addresses
	uintptr_t BaseAddress = (uintptr_t)GetModuleHandle(0);
	uintptr_t UWorld = FN->FindSignature("48 89 05 ? ? ? ? 48 8B 4B 78"); //ok
	uintptr_t UObject = FN->FindSignature("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"); //ok
	uintptr_t Free = FN->FindSignature("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"); //ok
	uintptr_t WorldToScreen = FN->FindSignature("E8 ? ? ? ? 48 8B 5C 24 ? 41 88 07 48 83 C4 30"); //ok
	uintptr_t GetNameByIndex = FN->FindSignature("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B F2 4C 8B F1 E8 ? ? ? ? 45 8B 06 33 ED 41 0F B7 16 41 C1 E8 10 89 54 24 24 44 89 44 24 ? 48 8B 4C 24 ? 48 C1 E9 20 8D 3C 09 4A 03 7C C0 ? 0F B7 17 C1 EA 06 41 39 6E 04"); //ok


	//Resolve Relative Addresses
	UWorld = RELATIVE(UWorld, 7);
	UObject = RELATIVE(UObject, 7);
	WorldToScreen = RELATIVE(WorldToScreen, 5);

	//Subtract the image base
	UWorld -= BaseAddress;
	UObject -= BaseAddress;
	Free -= BaseAddress;
	WorldToScreen -= BaseAddress;
	GetNameByIndex -= BaseAddress;

	//Print them
	std::cout << "  [BaseAddress] 0x" << BaseAddress << "\n";
	std::cout << "  [UWorld] 0x" << UWorld << "\n";
	std::cout << "  [UObject] 0x" << UObject << "\n";
	std::cout << "  [Free] 0x" << Free << "\n";
	std::cout << "  [WorldToScreen] 0x" << WorldToScreen << "\n";
	std::cout << "  [GetNameByIndex] 0x" << GetNameByIndex << "\n";
}

VOID Main()
{
	//Yes they are deprecated just stfu
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	//Yes change it = gay
	SetConsoleTitleA("FNTool | android#1337");
	

	const char* GObjects_Signature = "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1";
	const char* GetNameByIndex_Signature = "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B F2 4C 8B F1 E8 ? ? ? ? 45 8B 06 33 ED 41 0F B7 16 41 C1 E8 10 89 54 24 24 44 89 44 24 ? 48 8B 4C 24 ? 48 C1 E9 20 8D 3C 09 4A 03 7C C0 ? 0F B7 17 C1 EA 06 41 39 6E 04";
	const char* FnFree_Signature = "48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0";

	//Initialize the tool
	FNTool* FN = new FNTool();
	FN->Initialize(GObjects_Signature, GetNameByIndex_Signature, FnFree_Signature);


	//Dump basic offsets
	FN->FindAndLogOffset("World", "Levels");
	FN->FindAndLogOffset("World", "PersistentLevel");
	FN->FindAndLogOffset("GameInstance", "LocalPlayers");
	FN->FindAndLogOffset("World", "OwningGameInstance");
	FN->FindAndLogOffset("Controller", "ControlRotation");
	FN->FindAndLogOffset("PlayerController", "PlayerCameraManager");
	FN->FindAndLogOffset("PlayerController", "AcknowledgedPawn");
	FN->FindAndLogOffset("Pawn", "PlayerState");
	FN->FindAndLogOffset("Actor", "RootComponent");
	FN->FindAndLogOffset("Character", "Mesh");
	FN->FindAndLogOffset("SceneComponent", "RelativeLocation");
	FN->FindAndLogOffset("SceneComponent", "ComponentVelocity");
	FN->FindAndLogOffset("StaticMeshComponent", "StaticMesh");
	FN->FindAndLogOffset("StaticMeshComponent", "ComponentToWorld");
	FN->FindAndLogOffset("SkinnedMeshComponent", "CachedWorldSpaceBounds");
	FN->FindAndLogOffset("Actor", "CustomTimeDilation");
	FN->FindAndLogOffset("FortWeapon", "LastFireTimeVerified");
	FN->FindAndLogOffset("FortWeapon", "LastFireTime");
	FN->FindAndLogOffset("FortPawn", "bIsDBNO");
	FN->FindAndLogOffset("FortPawn", "bIsDying");
	FN->FindAndLogOffset("FortPlayerStateAthena", "TeamIndex");
	FN->FindAndLogOffset("FortPickup", "PrimaryPickupItemEntry");
	FN->FindAndLogOffset("FortItemDefinition", "DisplayName");
	FN->FindAndLogOffset("FortItemDefinition", "Tier");
	FN->FindAndLogOffset("FortItemEntry", "ItemDefinition");
	FN->FindAndLogOffset("FortPawn", "CurrentWeapon");
	FN->FindAndLogOffset("FortWeapon", "WeaponData");
	FN->FindAndLogOffset("Pawn", "RemoteViewPitch");
	FN->FindAndLogOffset("FortWeapon", "LastFireAbilityTime");
	FN->FindAndLogOffset("FortWeaponItemDefinition", "WeaponStatHandle");
	FN->FindAndLogOffset("FortProjectileAthena", "FireStartLoc");
	FN->FindAndLogOffset("FortBaseWeaponStats", "ReloadTime");
	FN->FindAndLogOffset("FortBaseWeaponStats", "ReloadScale");
	FN->FindAndLogOffset("FortBaseWeaponStats", "ChargeDownTime");
	FN->FindAndLogOffset("FortRangedWeaponStats", "RecoilHoriz");
	FN->FindAndLogOffset("FortRangedWeaponStats", "RecoilVert");
	FN->FindAndLogOffset("FortRangedWeaponStats", "RecoilDownsightsMultiplier");
	FN->FindAndLogOffset("FortPlayerPawnAthena", "ReviveFromDBNOTime");
	FN->FindAndLogOffset("FortPlayerPawnAthena", "bADSWhileNotOnGround");
	FN->FindAndLogOffset("FortWeapon", "WeaponReloadMontage");
	FN->FindAndLogOffset("FortWeapon", "ReloadAnimation");

	restart:
	std::cout << "\n  [FNTool] Please input the command: ";
	std::string input;
	std::cin >> input;
	std::cout << "\n";

	if (input == "offset")
	{
		FindCustomOffset(FN);
	}
	else if (input == "class")
	{
		FindCustomClass(FN);
	}
	else if (input == "mainoffsets")
	{
		FindMainOffsets(FN);
	}
	else if (input == "help")
	{
		std::cout << "  offset -> dump an offset (example: FortPawn.bIsDBNO)\n";
		std::cout << "  mainoffsets -> dump main offsets (example: UWorld, gobjects...)\n";
		std::cout << "  class -> dump an entire class (example: FortPawn)\n";
		std::cout << "  exit -> close the game\n";
	}
	else if (input == "exit")
		exit(0);
	else
		std::cout << "  [!] Failed\n";


	goto restart;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason != DLL_PROCESS_ATTACH) return FALSE;

	Main();

	return TRUE;
}