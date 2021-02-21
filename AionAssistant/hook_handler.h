#pragma once
#include <Windows.h>

// Function headers
void __stdcall Eject(const char* msg);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void SetMousePosition();

class AAGUI {
private:

public:
	static void __stdcall Initialize(HMODULE lpParameter);
	static void ThisThing();
};