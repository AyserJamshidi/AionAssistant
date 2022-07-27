// Declare project libraries
#pragma comment(lib, "dependencies/detours/detours.lib")
//#pragma comment(lib, "dependencies/Lua542/liblua54.a")
#pragma comment(lib, "dependencies/traypp/tray.lib")

#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <iostream> 
#include <thread>
#include <Psapi.h>
#include <unordered_map>

#include "Dependencies/detours/detours.h"
#include "include/thread_managers/threads.hpp"
#include "src/memory/memory.hpp"
#include "src/structures/internalstructures.hpp"
#include "src/helpers/helpers.hpp"

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
GlobalVars globalVars;
bool isRunning = false; // Program threads rely on this to stay on/turn off
int currentTime = 0; // Used as a time reference, increments 10 times a second

// Entity containers
uintptr_t playerEntity;
std::unordered_map<uintptr_t, int> entityMap;


__declspec(dllexport) void EntityIntercept() {
	entityMap[entityAddress] = currentTime;
}


void Initialize() {
	isRunning = true;
	DEBUG_PRINT("Initializing...\n");
	globalVars = { &isRunning, &currentTime };

	// Start internal timer thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::TimeUpdater, &globalVars, 0, 0));

	// Start entity map cleaner thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::EntityMapManager, new EntityMapStructure{ &globalVars, &playerEntity, &entityMap}, 0, 0));

	// Start player hotkey thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::SelfHotkeyManager, new HotkeyStructure{ &globalVars, &playerEntity }, 0, 0));

	// Start tray manager thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::TrayManager, &globalVars, 0, 0));

	// Start input manager thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::InputManager, 0, 0, 0));

	// Get CryEntitySystem module
	HMODULE cryEntitySystemModule = GetModuleHandleA("CryEntitySystem.dll");

	if (cryEntitySystemModule == nullptr) {
		printf("Could not find CryEntitySystem.dll base address.\n");
		return;
	}

	// Byte pattern scan  our entry point
	detourStartAddress = Memory::Accessor::FindPattern(cryEntitySystemModule, "\x48\x8B\x40\x20\x48\x83\xC4\x20\x5B\xC3\xCC\xCC", "xxxxxxxxxxxx");

	if (detourStartAddress == 0) {
		printf("Could not find pattern\n");
		return;
	}

	// Assign address after our detoured overwritten bytes
	printf("Address %08llX\n", detourStartAddress);
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
	printf("[-] Entity redirect complete!\n");
}

void Eject() {
	if (!isRunning) {
		printf("isRunning == false, rejecting isRunning == false request!\n");
		return;
	}

	printf("Unloading...\n");
	isRunning = false;
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)detourStartAddress, my_hook);
	DetourTransactionCommit();
	printf("Done!\n");
}

void MainLoop(LPVOID lpThreadParameter) {
	while (!GetModuleHandleW(L"CryEntitySystem.dll")) {
		printf("Waiting for CryEntitySystem.dll...\n");
		Sleep(1000);
	}

	Initialize();

	printf("Starting main loop!\n");
	while (true) {
		if (GetKeyState(VK_RIGHT) & 0x8000)
			Eject();

		if (GetKeyState(VK_UP) & 0x8000) {
			if (isRunning) {
				printf("isRunning == true, rejecting init request!\n");
				continue;
			}

			Initialize();
		}

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
		if (GetKeyState(VK_DOWN) & 0x8000) {
			if (isRunning) {
				DEBUG_PRINT("isRunning == true, rejecting quit request!\n");
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
		//printf("Waiting for CryEntitySystem.dll...\n");
		Sleep(1000);
		waitedTime++;
	}

	/* Will return false if we've exceeded our timer.
	 *
	 * Typically will only return false if Aion has crashed or hangs forever during startup.
	 */
	return waitedTime < 60;
}

int __stdcall DllMain(_In_ HMODULE hModule, _In_ DWORD fdwReason, _In_ LPVOID lpReserved) {
	//DisableThreadLibraryCalls(hModule);

	if (fdwReason == DLL_PROCESS_ATTACH) {
		AllocConsole();
		FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		//hwnd = Helpers::GetMainHWND();

		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainLoop, hModule, 0, 0));
	}
	else
		FreeLibrary(hModule);

	return true;
}