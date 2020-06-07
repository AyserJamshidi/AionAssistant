#pragma once
#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);
BOOL dllTargetIsForeground(int givenProcessID);
HMODULE GetThisDllHandle();
DWORD WINAPI DirectXInit(__in  LPVOID lpParameter);