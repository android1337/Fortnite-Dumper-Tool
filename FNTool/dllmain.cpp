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
	uintptr_t GObject = FN->FindSignature("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"); //ok
	uintptr_t Free = FN->FindSignature("E8 ? ? ? ? 48 8B 4C 24 ? 48 85 C9 0F 84 ? ? ? ? E8 ? ? ? ? 90 E9 ? ? ? ? 48 8D 15 ? ? ? ? 83 CB 08 4C 8B"); //ok
	uintptr_t GetNameByIndex = FN->FindSignature("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 01 4C 8B F2"); //ok


	//Resolve Relative Addresses
	UWorld = RELATIVE(UWorld, 7);
	GObject = RELATIVE(GObject, 7);
	Free = RELATIVE(Free, 5);

	//Subtract the image base
	UWorld -= BaseAddress;
	GObject -= BaseAddress;
	Free -= BaseAddress;
	GetNameByIndex -= BaseAddress;

	//Print them
	std::cout << "  [BaseAddress] 0x" << BaseAddress << "\n";
	std::cout << "  [UWorld] 0x" << UWorld << "\n";
	std::cout << "  [GObject] 0x" << GObject << "\n";
	std::cout << "  [Free] 0x" << Free << "\n";
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
	const char* GetNameByIndex_Signature = "48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 01 4C 8B F2";
	const char* FnFree_Signature = "E8 ? ? ? ? 48 8B 4C 24 ? 48 85 C9 0F 84 ? ? ? ? E8 ? ? ? ? 90 E9 ? ? ? ? 48 8D 15 ? ? ? ? 83 CB 08 4C 8B";

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
