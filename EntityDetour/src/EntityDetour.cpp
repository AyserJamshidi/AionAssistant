#pragma comment(lib, "../Dependencies/detours/detours.lib")

#include "../include/EntityDetour.hpp"
#include "../../Dependencies/detours/detours.h"

#include <stdio.h>
#include <unordered_map>

extern "C" { // ASM Accessable
	__declspec(dllexport) uintptr_t entityAddress;
	__declspec(dllexport) uintptr_t aiondetour_EndAddress;
	__declspec(dllexport) void EntityIntercept();

	void entityDetourCode();
}

uintptr_t aiondetour_StartAddress;
std::unordered_map<uintptr_t, int>* aiondetour_EntityMap;
int* time;

__declspec(dllexport) void EntityIntercept() {
	(*aiondetour_EntityMap)[entityAddress] = *time; //currentTime;
}

void AionDetour::SetStartAddress(uintptr_t givenAddress) {
	aiondetour_StartAddress = givenAddress;
}

void AionDetour::SetEndAddress(uintptr_t givenAddress) {
	aiondetour_EndAddress = givenAddress;
}

void AionDetour::SetMapAddress(std::unordered_map<uintptr_t, int>* givenMap) {
	aiondetour_EntityMap = givenMap;
}

void AionDetour::SetTimeAddress(int* givenTime) {
	time = givenTime;
}

bool AionDetour::Initiate() {
	if (aiondetour_StartAddress == 0) {
		printf("[-] Start address is not set for detour.");
		return false;
	}

	if (aiondetour_EndAddress == 0) {
		printf("[-] End address is not set for detour.");
		return false;
	}

	if (aiondetour_EntityMap == 0) {
		printf("[-] Failed to detour");
		return false;
	}

	// Initiate hook
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)aiondetour_StartAddress, entityDetourCode);

	LONG lError = DetourTransactionCommit();

	if (lError != NO_ERROR) {
		printf("[-] Failed to detour");
		return false;
	}
	printf("[+] Entity redirect complete!\n");
}

void AionDetour::Terminate() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)aiondetour_StartAddress, entityDetourCode);
	DetourTransactionCommit();
}