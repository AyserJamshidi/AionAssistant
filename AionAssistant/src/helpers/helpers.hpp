#pragma once
#include <Windows.h>
#include <string>

#define DEBUG 1

#ifdef DEBUG
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
#endif

class Helpers {
public:
	static HWND GetMainHWND();
};