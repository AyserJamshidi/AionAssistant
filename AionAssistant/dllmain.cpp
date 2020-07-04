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

void DetourRemove(void* pSource, int dwLen) {
	int MinLen = 14;

	if (dwLen < MinLen)
		return;

	if (*(WORD*)(pSource) != 0x25FF)
		return;

	void* pTrampoline = *(void**)((uintptr_t)pSource + 6);

	DWORD oldProtect = 0;
	VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy(pSource, pTrampoline, dwLen);

	VirtualProtect(pSource, dwLen, oldProtect, &oldProtect);
}

void* DetourFunction(void* pSource, void* pDestination, int dwLen) {
	int MinLen = 14;

	/* If length of bytes we are stealing is < 14 bytes we SHOULD NOT modify anything.
	 *
	 * 64-bit ASM requires at least 14 bytes to detour as that is the bare minimum for
	 * the QWORD pointer we are going to redirect to.
	 * 
	 * Continuing with < 14 bytes will corrupt the stack and crash Aion.
	 */
	if (dwLen < MinLen)
		return nullptr;

	BYTE stub[] = {
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,                // jmp qword ptr [$+6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     // ptr that we input
	};

	// Gets address for detour code
	void* pTrampoline = VirtualAlloc(0, dwLen + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	DWORD dwOld = 0;
	VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

	// trampoline, skips 6 bytes then places pDestination addr
	memcpy(stub + 6, &pDestination, 8);
	//memcpy(stub + 6, pSource, sizeof(pSource));

	// Copy ASM to destination
	memcpy((void*)((DWORD_PTR)pTrampoline), pSource, dwLen);

	// JMP placement.  First var is location to place, second var is bytes to place, third var is size.
	// stub var seems to have some corruption at this point?
	memcpy((void*)((DWORD_PTR)pTrampoline + dwLen), stub, sizeof(stub));

	// orig
	memcpy(stub + 6, &pTrampoline, 8);

	// Replaces pSource with JMP to detour
	memcpy(pSource, stub, sizeof(stub));

	// Apply NOPs if we didn't use enough bytes yet
	for (int i = MinLen; i < dwLen; i++)
		*(BYTE*)((DWORD_PTR)pSource + i) = 0x90;

	VirtualProtect(pSource, dwLen, dwOld, &dwOld);

	return (void*)((DWORD_PTR)pSource + dwLen);
}

bool Hook(BYTE* pTarget, BYTE* pHook, UINT Length) {
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
}

void interceptEntities(uintptr_t givenAddress) {
	printf("%I64x\n", givenAddress);
}

void Blah() {
	while (!GetModuleHandleW(L"CryEntitySystem.dll")) {
		printf("Waiting for Aion init.\n");
		Sleep(100);
	}

	printf("Hooked!\n");

	uintptr_t entityHandle = (uintptr_t)GetModuleHandleW(L"CryEntitySystem.dll");

	if (entityHandle != NULL) {
		uintptr_t initialAddr = entityHandle + 0x5A107;
		int byteLength = 16;
		jmpBackAddr = initialAddr + byteLength;

		DetourFunction((LPVOID)(initialAddr), (LPVOID)AION, byteLength);
	}

	//DetourFunction((LPVOID)0x7FF67E733070, (LPVOID)AION, 18);
	//std::cout << "Bool = " << Hook((BYTE*)0x7FFCC361FE38, (BYTE*)AION, 16);
}

int __stdcall DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {

	//DisableThreadLibraryCalls(hModule);

	if (fdwReason == DLL_PROCESS_ATTACH) {
		AllocConsole();
		FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
		freopen_s(&pCout, "CONOUT$", "w", stdout);

		//HANDLE entityLoopHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Blah, 0, 0, 0);
		//std::thread(Blah).detach();
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DirectXInit, hModule, 0, 0));	
	}

	return true;
}
