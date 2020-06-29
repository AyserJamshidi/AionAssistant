/*// dllmain.cpp : Defines the entry point for the DLL application.
#include "dllmain.h"
#include <iostream>

#include "MinHook/include/MinHook.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {

	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		AllocConsole();
		FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
		freopen_s(&pCout, "CONOUT$", "w", stdout);

		//CloseHandle(CreateThread(NULL, NULL, DirectXInit, hModule, NULL, NULL));
		CreateThread(NULL, NULL, DirectXInit, hModule, NULL, NULL);
		//DisableThreadLibraryCalls(hModule);
	}

	return TRUE;
}*/

#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

extern "C" void writei(DWORD_PTR i);
extern "C" DWORD_PTR jmpBackAddr;

DWORD_PTR jmpBackAddr;

extern "C" void AION();
extern "C" void F1();

void DetourRemove(void* pSource, int dwLen) {
	int MinLen = 14;

	if (dwLen < MinLen)
		return;

	if (*(WORD*)(pSource) != 0x25FF)
		return;

	void* pTrampoline = *(void**)((DWORD_PTR)pSource + 6);

	DWORD oldProtect = 0;
	VirtualProtect(pSource, dwLen, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy(pSource, pTrampoline, dwLen);

	VirtualProtect(pSource, dwLen, oldProtect, &oldProtect);
}

void* DetourFunction(void* pSource, void* pDestination, int dwLen) {
	int MinLen = 14;

	if (dwLen < MinLen)
		return NULL;

	BYTE stub[] = {
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,                // jmp qword ptr [$+6]
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00     // ptr
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
	for (int i = MinLen; i < dwLen; i++) {
		*(BYTE*)((DWORD_PTR)pSource + i) = 0x90;
	}

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

//int xa = 0;
void writei( DWORD_PTR i ) {
	//xa++;
	std::cout << "Hi! = " << std::hex << i << std::endl;
	//Sleep(10);
}
void Blah() {
	while (!GetModuleHandle(L"CryEntitySystem.dll")) {
		std::cout << "Waiting for Aion init.";
		Sleep(100);
	}

	DWORD_PTR entityHandle = (DWORD_PTR)GetModuleHandle(L"CryEntitySystem.dll");

	if (entityHandle != NULL) {
		DWORD_PTR initialAddr = entityHandle + 0x5A107;
		int byteLength = 16;

		jmpBackAddr = initialAddr + byteLength;
		DetourFunction((LPVOID)(initialAddr), (LPVOID)AION, byteLength);
	}
	
	/*
	DetourFunction((LPVOID)0x7FF7FDE8319D, (LPVOID)AION, 17);*/
	//std::cout << "Bool = " << Hook((BYTE*)0x7FFCC361FE38, (BYTE*)AION, 16);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		AllocConsole();
		FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		
		std::cout << "Hi!";

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Blah, NULL, NULL, NULL);
		break;
	}

	return TRUE;
}
