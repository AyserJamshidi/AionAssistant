// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "dllmain.h"
#include <iostream>

#include "MinHook/include/MinHook.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	DisableThreadLibraryCalls(hModule);

	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		AllocConsole();
		FILE* pCout; // Dummy file so we can properly in/output to console while avoiding security issues from freopen(..).
		freopen_s(&pCout, "CONOUT$", "w", stdout);

		CloseHandle(CreateThread(NULL, NULL, DirectXInit, hModule, NULL, NULL));
	} else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
		std::cout << "BYE! 2" << std::endl;
	}

	return TRUE;
}