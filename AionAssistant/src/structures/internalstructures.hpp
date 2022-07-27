#pragma once
#include <unordered_map>
#include "aion.hpp"

struct GlobalVars {
	bool* isRunning;
	int* currentTime;
};

struct EntityMapStructure {
	GlobalVars* globalVars;
	uintptr_t* playerAddress;
	std::unordered_map<uintptr_t, int>* entityMap;
};

struct HotkeyStructure {
	GlobalVars* globalVars;
	uintptr_t* playerAddress;
};