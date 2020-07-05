#include "dllmain.h"

#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <thread>

extern "C" uintptr_t jmpBackAddr; // Placeholder for our ASM jump back address.
extern "C" void interceptEntities(uintptr_t givenAddress);

uintptr_t jmpBackAddr;

extern "C" void AION();




/*bool Hook(BYTE* pTarget, BYTE* pHook, UINT Length) {
	if (!pTarget || !pHook || Length < 14)
		return false;

	DWORD dwOld = 0;
	if (!VirtualProtect(pTarget, Length, PAGE_EXECUTE_READWRITE, &dwOld))
		return false;

	memset(pTarget, 0x90, Length);

#ifdef _WIN64
	memset(pTarget, 0x00, 14);
	*(pTarget + 0x00) = 0xFF;
	*(pTarget + 0x01) = 0x25;
	*reinterpret_cast<BYTE**>(pTarget + 0x06) = pHook;
#else
	* pTarget = 0xE9;
	*reinterpret_cast<DWORD*>(pTarget + 0x01) = pHook - pTarget - 5;
#endif
	VirtualProtect(pTarget, Length, dwOld, &dwOld);
	return true;
}*/

void interceptEntities(uintptr_t givenAddress) {
	printf("%I64x\n", givenAddress);
}

void Blah() {

	printf("Hooked!\n");

	uintptr_t entityHandle = (uintptr_t)GetModuleHandleW(L"CryEntitySystem.dll");

	if (entityHandle != NULL) {
		uintptr_t initialAddr = entityHandle + 0x5A107;
		int byteLength = 16;
		jmpBackAddr = initialAddr + byteLength;

		//DetourFunction((LPVOID)(initialAddr), (LPVOID)AION, byteLength);
	}

	//DetourFunction((LPVOID)0x7FF67E733070, (LPVOID)AION, 18);
	//std::cout << "Bool = " << Hook((BYTE*)0x7FFCC361FE38, (BYTE*)AION, 16);
}

int __stdcall DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {

	//DisableThreadLibraryCalls(hModule);

	if (fdwReason == DLL_PROCESS_ATTACH) {
		if (AionInitWaiter(60)) {
			AllocConsole();
			FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
			freopen_s(&pCout, "CONOUT$", "w", stdout);

			//std::thread(DirectXInit, hModule).detach();
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DirectXInit, hModule, 0, 0));
		} else {

		}


		//HANDLE entityLoopHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Blah, 0, 0, 0);
		//std::thread(Blah).detach();	
	}

	return true;
}

bool AionInitWaiter(int timeToWait) {
	int waitedTime = 0;

	while (!GetModuleHandleW(L"CryEntitySystem.dll") && waitedTime < timeToWait) {
		Sleep(1000);
		waitedTime++;
	}

	/* Will return false if we've exceeded our timer.
	 *
	 * Typically will only return false if Aion has crashed or hangs forever during startup.
	 */
	return waitedTime < 60;
}