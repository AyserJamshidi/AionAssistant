#include "threads.hpp"
#include "../structures/internalstructures.hpp"
#include "../structures/aion.hpp"
#include <locale>

void AionThreads::EntityMapManager(void* givenStruct) {
	EntityMapStructure entityMapStructure = *(EntityMapStructure*)givenStruct;


	while (*entityMapStructure.globalNeeds->isRunning) {
		for (auto entity : *entityMapStructure.entityMap) {
			AionEntity* ent = (AionEntity*)entity.first;

			if (*entityMapStructure.globalNeeds->currentTime - entity.second > 2) {
				entityMapStructure.entityMap->erase(entity.first);
			}

			printf("Type: %s\n", ent->Type);

			//if ( ent->actualEntity != 0 && ent.)
				//printf("%08llX -- NAME: %ls -- ANI SPEED: %i -- TYPE: %s\n", entity.first, ent->actualEntity->Name, ent->actualEntity->AnimationSpeed, ent->Type);
		}

		Sleep(1000); 
	}

	printf("EntityMapCleaner ended!\n");
}