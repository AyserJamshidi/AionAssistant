#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <unordered_map>


class AionDetour {
public:
	void SetStartAddress(uintptr_t start);
	void SetEndAddress(uintptr_t end);
	void SetMapAddress(std::unordered_map<uintptr_t, int>* map);
	void SetTimeAddress(int* givenTime);
	bool Initiate();
	void Terminate();
};