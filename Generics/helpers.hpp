#pragma once
#include <Windows.h>
#include <string>

// #define DEBUG 1

#ifdef _DEBUG
#define DEBUG_PRINT(...) {\
printf("[+] "); \
printf(__VA_ARGS__); \
}

#define DEBUG_PRINT_ERR(...) {\
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