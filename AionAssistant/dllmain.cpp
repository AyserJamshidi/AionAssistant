#pragma comment(lib, "Dependencies/detours/detours.lib")

#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <Psapi.h>
#include <unordered_map>

#include "Dependencies/detours/detours.h"
#include "src/threads/threads.hpp"
#include "src/memory/memory.hpp"
#include "src/internalstructures.h"

// ASM Accessable
extern "C" {
	__declspec(dllexport) uintptr_t entityAddress;
	__declspec(dllexport) uintptr_t detourEndAddress;
	__declspec(dllexport) void EntityIntercept();

	void my_hook();
}

// Non-passed variables
uintptr_t detourStartAddress;

// Passed-along variables
GlobalNeeds globalNeeds;
bool isRunning = false; // Program threads rely on this to stay on/turn off
int currentTime = 0; // Used as a time reference, incremented 1 every second
std::unordered_map<uintptr_t, int> entityMap;

__declspec(dllexport) void EntityIntercept() {
	entityMap[entityAddress] = currentTime;
	//printf("Entity at address: %08X, map size is %i with value %i\n", entityAddress, entityMap.size(), entityMap[entityAddress]);
}


void Initialize() {
	printf("Initializing...\n");
	globalNeeds = { &isRunning };
	
	// Start internal timer thread
	//TimeStructure* timeUpdaterVar = new TimeStructure{ &isRunning, &currentTime };
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionThreads::TimeUpdater, new TimeStructure{ &globalNeeds, &currentTime }, 0, 0));

	// Start entity map cleaner thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionThreads::EntityMapCleaner, new EntityMapStructure{ &globalNeeds, &entityMap }, 0, 0));

	// Get CryEntitySystem module
	HMODULE entityHandle = GetModuleHandleA("CryEntitySystem.dll");

	if (entityHandle == nullptr) {
		printf("Could not find CryEntitySystem.dll base address.\n");
		return;
	}

	// Byte pattern scan for our entry point
	detourStartAddress = Memory::Accessor::FindPattern(entityHandle, "\x48\x8B\x40\x20\x48\x83\xC4\x20\x5B\xC3\xCC\xCC", "xxxxxxxxxxxx");

	if (detourStartAddress == 0) {
		printf("Could not find pattern\n");
		return;
	}

	// Assign address after our detoured overwritten bytes
	std::cout << "Address: " << std::hex << detourStartAddress << std::endl;
	detourEndAddress = detourStartAddress + 8;

	// Initiate hook
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)detourStartAddress, my_hook);

	LONG lError = DetourTransactionCommit();

	if (lError != NO_ERROR) {
		printf("[-] Failed to detour");
		return;
	}
}

void MainLoop(LPVOID lpThreadParameter) {
	while (true) {
		printf("Hi!\n");

		if (GetKeyState(VK_RIGHT) & 0x8000) {
			printf("Unloading...\n");
			isRunning = false;
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(&(PVOID&)detourStartAddress, my_hook);
			DetourTransactionCommit();
			printf("Done!\n");
		}

		if (GetKeyState(VK_UP) & 0x8000) {
			isRunning = true;
			Initialize();
		}

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
		if (GetKeyState(VK_DOWN) & 0x8000) {
			if (isRunning) {
				printf("isRunning == true, rejecting quit request!\n");
				continue;
			}

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

		if (AionInitWaiter(60)) {
			printf("Aion is initialized!\n");

			std::cout << "Hello!" << std::endl;

			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainLoop, hModule, 0, 0));

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