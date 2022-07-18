#pragma once
#include <unordered_map>

struct GlobalNeeds {
	bool* isRunning;
};

struct TimeStructure {
	GlobalNeeds* globalNeeds;
	int* currentTime;
};

struct EntityMapStructure {
	GlobalNeeds* globalNeeds;
	std::unordered_map<uintptr_t, int>* entityMap;
};