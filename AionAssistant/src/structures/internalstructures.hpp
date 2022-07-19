#pragma once
#include <unordered_map>

struct GlobalNeeds {
	bool* isRunning;
	int* currentTime;
};

struct EntityMapStructure {
	GlobalNeeds* globalNeeds;
	std::unordered_map<uintptr_t, int>* entityMap;
};