#include "memory.hpp"

#include <Windows.h>
#include <Psapi.h>

uintptr_t __stdcall Memory::Accessor::FindPattern(HMODULE handle, const char* pattern, const char* mask) {
	MODULEINFO moduleInfo = { 0 };

	GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(MODULEINFO));

	// Assign our base and module size to prevent accessing unwanted memory
	uintptr_t base = (uintptr_t)moduleInfo.lpBaseOfDll;
	uintptr_t size = (uintptr_t)moduleInfo.SizeOfImage;

	// Get length for our mask, this will allow us to loop through our array
	uintptr_t patternLength = (uintptr_t)strlen(mask);

	for (uintptr_t i = 0; i < size - patternLength; i++) {
		bool found = true;
		for (uintptr_t j = 0; j < patternLength; j++) {
			//if we have a ? in our mask then we have true by default,
			//or if the bytes match then we keep searching until finding it or not
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}

		// Pattern was found, return the address where it begins.
		if (found)
			return base + i;
	}

	return NULL;
}