#pragma once

#include <Windows.h>

class Memory {
public:
	class Accessor {
	public:
		static uintptr_t FindPattern(HMODULE handle, const char* pattern, const char* mask);
	};
};