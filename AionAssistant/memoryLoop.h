#pragma once
#include <Windows.h>

DWORD GetPointerAddress(int PointerLevel, DWORD Offsets[], DWORD baseAddress);

class MemLoop {
private:

public:
	void MainLoop();
};