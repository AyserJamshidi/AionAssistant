#include "AttachHelper.h"
#include <iostream>
#include <cassert>

BOOL dllTargetIsForeground(int givenProcessID) {
	DWORD curForegroundWnd;
	GetWindowThreadProcessId(GetForegroundWindow(), &curForegroundWnd);
	return givenProcessID == curForegroundWnd;
}

HMODULE GetThisDllHandle() {
	MEMORY_BASIC_INFORMATION info;
	size_t len = VirtualQueryEx(GetCurrentProcess(), (void*)GetThisDllHandle, &info, sizeof(info));
	assert(len == sizeof(info));
	return len ? (HMODULE)info.AllocationBase : NULL;
}