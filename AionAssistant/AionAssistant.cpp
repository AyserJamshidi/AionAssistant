// Declare project libraries
#pragma comment(lib, "../x64/Release/EntityDetour.lib")
#pragma comment(lib, "../Dependencies/Lua542/liblua54.a")
#pragma comment(lib, "../Dependencies/traypp/tray.lib")

// General dependencies
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <iostream> 
#include <thread>
#include <Psapi.h>
#include <unordered_map> 

// Internal dependencies
#include "../EntityDetour/include/EntityDetour.hpp"
#include "include/thread_managers/threads.hpp"
#include "src/memory/memory.hpp"
#include "src/structures/internalstructures.hpp"
#include "src/helpers/helpers.hpp"

// Non-passed variables
AionDetour aionDetour;

// Passed-along variables
GlobalVars globalVars;
bool isRunning = false; // Program threads rely on this to stay on/turn off
int currentTime = 0; // Used as a time reference, increments 10 times a second

// Entity containers
uintptr_t playerEntity;
std::unordered_map<uintptr_t, int> entityMap;

void Initialize() {
	isRunning = true;
	DEBUG_PRINT_ERR("Initializing...\n");
	globalVars = { &isRunning, &currentTime };

	// Start internal timer thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::TimeUpdater, &globalVars, 0, 0));

	// Start entity map cleaner thread
	//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::EntityMapManager, new EntityMapStructure{ &globalVars, &playerEntity, &entityMap}, 0, 0));

	// Start player hotkey thread
	//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::InputSelfManager, new HotkeyStructure{ &globalVars, &playerEntity }, 0, 0));

	// Start tray manager thread
	//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::TrayManager, &globalVars, 0, 0));

	// Start input manager thread
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantThread::InputManager, 0, 0, 0));

	// Get CryEntitySystem module
	HMODULE cryEntitySystemModule = GetModuleHandleA("CryEntitySystem.dll");

	if (cryEntitySystemModule == nullptr) {
		DEBUG_PRINT_ERR("Could not find CryEntitySystem.dll base address.\n");
		return;
	}

	// Byte pattern scan  our entry point
	uintptr_t detourStartAddress = Memory::Accessor::FindPattern(cryEntitySystemModule, "\x48\x8B\x40\x20\x48\x83\xC4\x20\x5B\xC3\xCC\xCC", "xxxxxxxxxxxx");

	if (detourStartAddress == 0) {
		DEBUG_PRINT_ERR("Could not find pattern\n");
		return;
	}

	aionDetour.SetStartAddress(detourStartAddress);
	aionDetour.SetEndAddress(detourStartAddress + 8);
	aionDetour.SetMapAddress(&entityMap);
	aionDetour.SetTimeAddress(&currentTime);

	aionDetour.Initiate();
}

void Eject() {
	if (!isRunning) {
		DEBUG_PRINT_ERR("isRunning == false, rejecting isRunning == false request!\n");
		return;
	}

	DEBUG_PRINT("Unloading...\n");
	isRunning = false;

	aionDetour.Terminate();
	DEBUG_PRINT("Done!\n");
}

void MainLoop(LPVOID lpThreadParameter) {
	while (!GetModuleHandleW(L"CryEntitySystem.dll")) {
		DEBUG_PRINT("Waiting for CryEntitySystem.dll...\n");
		Sleep(1000);
	}

	Initialize();

	DEBUG_PRINT("Starting main loop!\n");
	while (true) {
		if (GetKeyState(VK_RIGHT) & 0x8000)
			Eject();

		if (GetKeyState(VK_UP) & 0x8000) {
			if (isRunning) {
				DEBUG_PRINT_ERR("isRunning == true, rejecting init request!\n");
				continue;
			}

			Initialize();
		}

		// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
		if (GetKeyState(VK_DOWN) & 0x8000) {
			if (isRunning) {
				DEBUG_PRINT_ERR("isRunning == true, rejecting quit request!\n");
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
		DEBUG_PRINT("Waiting for CryEntitySystem.dll...\n");
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