#pragma once
#include <Windows.h>
#include <string>

// #define DEBUG 1

#ifdef _DEBUG
#define DEBUG_PRINT(...) {\
SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11); \
printf("[+] "); \
printf(__VA_ARGS__); \
}

#define DEBUG_PRINT_ERR(...) {\
SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); \
printf("[-] "); \
printf(__VA_ARGS__); \
}

#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINT_ERR(...)
#endif

class Helpers {
public:
	static HWND GetMainHWND();
};