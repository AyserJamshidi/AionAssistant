#pragma once
#include <Windows.h>

class EntityHandler {
private:

public:
	static void Initialize(uintptr_t addressOffset);
	static void Uninitialize();
};