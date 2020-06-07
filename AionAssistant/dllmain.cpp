// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "dllmain.h"
#include <iostream>

#include "MinHook/include/MinHook.h"


#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	DisableThreadLibraryCalls(hModule);

	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		#ifdef _DEBUG
		AllocConsole();
		FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
		freopen_s(&pCout, "CONIN$", "r", stdin);
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		#endif

		// We have to wait for DX9 to init or else we'll most likely experience odd behavior.
		while (!GetModuleHandle(L"d3d9.dll"))
			Sleep(100);

		CreateThread(NULL, NULL, DirectXInit, hModule, NULL, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}