#pragma once
#include <Windows.h>
#include <vector>
#include "../Models/AionEntity.h"

std::vector<AionEntity *> *entity_array_ptr;

class EntityHandler {
private:

public:
	EntityHandler(std::vector<AionEntity *> *received_ptr) {
		entity_array_ptr = received_ptr;
		std::cout << "Hi!";
	}

	static void Initialize(uintptr_t address_offset);
	static void Uninitialize();
};