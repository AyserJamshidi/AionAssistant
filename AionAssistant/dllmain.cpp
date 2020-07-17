#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <thread>

#include "hook_handler.h"
#include "Handlers/entity_handler.h"

void AionAssistantInitialize(HMODULE hModule);
bool AionInitWaiter(int timeToWait);

int __stdcall DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {

	//DisableThreadLibraryCalls(hModule);

	if (fdwReason == DLL_PROCESS_ATTACH) {
		//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AionAssistantInitialize, hModule, 0, 0));
		std::thread(AionAssistantInitialize, hModule).detach();
	}

	return true;
}

void AionAssistantInitialize(HMODULE hModule) {
	AllocConsole();
	FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
	freopen_s(&pCout, "CONOUT$", "w", stdout);

	if (AionInitWaiter(60)) {
		printf("Aion is initialized!\n");

		AAGUI::Initialize(hModule);
		//EntityHandler::Initialize(0x5A107);
		//EntityHandler::Uninitialize();
		//std::thread(AAGUI::Initialize, hModule).detach();
		//std::thread(EntityHandler::Initialize, 0x5A107).detach();
	} else {
		// TODO: Send a MessageBox (or anything) to the user with error message.
		FreeLibraryAndExitThread(hModule, 0);
	}
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