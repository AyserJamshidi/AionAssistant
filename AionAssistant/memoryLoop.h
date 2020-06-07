#pragma once
#include <Windows.h>

class MemLoop {
private:

public:
	DWORD GetPtrAddress(int PointerLevel, DWORD Offsets[], DWORD baseAddress, HANDLE &pHandle);
	void MainLoop();
};