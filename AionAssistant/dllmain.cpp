#pragma comment(lib, "detours/detours.lib")

#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <Psapi.h>

#include "detours/detours.h"

extern "C" {
	__declspec(dllexport) uintptr_t entityAddr;
	__declspec(dllexport) uintptr_t jmpBackAddr;
	__declspec(dllexport) void EntityIntercept();

	void my_hook();
}

MODULEINFO moduleInfo = { 0 };

__declspec(dllexport) void EntityIntercept() {
	//std::cout << "Intercepted" << std::endl;
}

uintptr_t FindPattern(const char* pattern, const char* mask) {
	// Assign our base and module size to prevent accessing unwanted memory
	uintptr_t base = (uintptr_t)moduleInfo.lpBaseOfDll;
	uintptr_t size = (uintptr_t)moduleInfo.SizeOfImage;

	// Get length for our mask, this will allow us to loop through our array
	uintptr_t patternLength = (uintptr_t)strlen(mask);

	for (uintptr_t i = 0; i < size - patternLength; i++) {
		bool found = true;
		for (uintptr_t j = 0; j < patternLength; j++) {
			//if we have a ? in our mask then we have true by default,
			//or if the bytes match then we keep searching until finding it or not
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}

		// Pattern was found, return the address where it begins.
		if (found)
			return base + i;
	}

	return NULL;
}

uintptr_t entityArrAddy;

void Initialize() {
	std::cout << std::hex;

	HMODULE entityHandle = GetModuleHandleA("CryEntitySystem.dll");

	if (entityHandle == nullptr) {
		printf("Could not find CryEntitySystem.dll base address.");
		return;
	}

	GetModuleInformation(GetCurrentProcess(), entityHandle, &moduleInfo, sizeof(MODULEINFO));

	entityArrAddy = FindPattern("\x48\x8B\x40\x20\x48\x83\xC4\x20\x5B\xC3", "xxxxxxxxxx");
	//uintptr_t entityArrAddy = FindPattern("\x48\x8B\x40\x20\x48\x8B\x7C\x24\x58", "xxxxxxxxx") + 4;

	std::cout << "Address: " << std::hex << entityArrAddy << std::endl;
	jmpBackAddr = entityArrAddy + 8;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)entityArrAddy, my_hook);

	LONG lError = DetourTransactionCommit();

	if (lError != NO_ERROR) {
		printf("[-] Failed to detour");
		return;
	}
}

boolean keepLooping = false;

void EntityCheckerThread() {
	while (keepLooping) {
		if (entityAddr != 0) {
			printf("Entity at address: %08X\n", entityAddr);

			entityAddr = 0;
		}
	}
}

void StartAddress(LPVOID lpThreadParameter) {
	while (true) {
		printf("Hi!\n");

		if (GetKeyState(VK_RIGHT) & 0x8000) {
			printf("Unloading...\n");
			keepLooping = false;
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(&(PVOID&)entityArrAddy, my_hook);
			DetourTransactionCommit();
			printf("Done!\n");
		}

		if (GetKeyState(VK_UP) & 0x8000) {
			keepLooping = true;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)EntityCheckerThread, lpThreadParameter, 0, 0);
			Initialize();
		}

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
		if (GetKeyState(VK_DOWN) & 0x8000) {
			fclose((FILE*)stdout);
			FreeConsole(); // Free the console.
			FreeLibraryAndExitThread((HMODULE)lpThreadParameter, 0);
		}

		Sleep(1000);
	}

	return;
}

bool AionInitWaiter(int timeToWait) {
	int waitedTime = 0;

	while (!GetModuleHandleW(L"CryEntitySystem.dll") && waitedTime < timeToWait) {
		printf("Waiting for CryEntitySystem.dll...\n");
		Sleep(1000);
		waitedTime++;
	}

	//TODO: Make sure the game is ready for the hack by 

	/* Will return false if we've exceeded our timer.
	 *
	 * Typically will only return false if Aion has crashed or hangs forever during startup.
	 */
	return waitedTime < 60;
}

void AionAssistantInitialize(HMODULE hModule) {
	AllocConsole();
	FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
	freopen_s(&pCout, "CONOUT$", "w", stdout);

	if (AionInitWaiter(60)) {
		printf("Aion is initialized!\n");

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Initialize, hModule, 0, 0);
	} else {
		// TODO: Send a MessageBox (or anything) to the user with error message.
		//system("PAUSE");
		//FreeLibraryAndExitThread(hModule, 0);
	}
}

int __stdcall DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
	//DisableThreadLibraryCalls(hModule);

	if (fdwReason == DLL_PROCESS_ATTACH) {
		AllocConsole();
		FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
		freopen_s(&pCout, "CONOUT$", "w", stdout);

		if (true /*AionInitWaiter(60)*/) {
			printf("Aion is initialized!\n");

			std::cout << "Hello!" << std::endl;

			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartAddress, hModule, 0, 0));

			std::cout << "Done!" << std::endl;
			Sleep(1000);
		} else {
			// TODO: Send a MessageBox (or anything) to the user with error message.
			//system("PAUSE");
			//FreeLibraryAndExitThread(hModule, 0);
		}
		//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantInitialize, hModule, 0, 0));
	}

	return true;
}