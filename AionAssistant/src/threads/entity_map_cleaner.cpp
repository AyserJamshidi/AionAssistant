#include "threads.hpp"
#include "../internalstructures.h"

void AionThreads::EntityMapCleaner(void* givenStruct) {
	EntityMapStructure entityMapStructure = *(EntityMapStructure*)givenStruct;

	while (*entityMapStructure.globalNeeds->isRunning) {

		std::unordered_map<int, int> m;

		for (auto entity : *entityMapStructure.entityMap) {
			printf("Scanning %08X\n", entity.first);
		}

		Sleep(1000);
	}

	printf("EntityMapCleaner ended!\n");
}