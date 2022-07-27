#include "../../include/thread_managers/threads.hpp"
#include "../structures/internalstructures.hpp"
#include "../structures/aion.hpp"
#include "../helpers/helpers.hpp"

#include <format>
#include <locale>


bool CDCDScan(CEntity* entity) {
	for (int i = 0; i < 4; i++)
		if ((((uintptr_t)(entity->EntityObject) >> (8 * i)) & 0xff) == 0xCD)
			return false;
}

void AionAssistantThread::EntityMapManager(void* givenStruct) {
	EntityMapStructure entityMapStructure = *(EntityMapStructure*)givenStruct;
	DEBUG_PRINT("EntityMapManager started!\n");

	std::unordered_map<uintptr_t, int>* map = entityMapStructure.entityMap;

	setlocale(LC_ALL, "");

	while (*entityMapStructure.globalVars->isRunning) {
		// Must use iterator as range-based loops will cause crashing upon removing an entity
		for (auto entity = (*map).cbegin(); entity != (*map).cend();) {

			// Ensure the entity has updated itself in the last 2 to 3 seconds
			if (*(entityMapStructure.globalVars->currentTime) - entity->second > 3) {
				// Entity has not updated itself.  Remove it from our entity map.
				map->erase(entity++->first);
				continue;
			}
			
			// 
			CEntity* ent = (CEntity*)entity->first;

			if (ent->IsValidEntity() && ent->IsValidEntityObject()) {
				DEBUG_PRINT("%08llX -- %ls -- ANI: %i\n", (uintptr_t)ent, ent->EntityObject->Name, ent->EntityObject->AnimationSpeed);

				// If this entity is of type <Player>, it us the client's primary entity, and we store it.
				if (std::strcmp(ent->Type, "Player") == 0) {
					*entityMapStructure.playerAddress = (uintptr_t)ent;
					DEBUG_PRINT("Set player entity!\n");
				}

				//if (ent->actualEntity != 0 /* && ent->Type != 0 && std::strcmp(ent->Type, "User")*/)
				//	printf("%08llX -- NAME: %ls -- ANI SPEED: %i -- TYPE: %s -- POSITION: %s\n", entity.first/*, ent->actualEntity->Name, ent>actualEntity->AnimationSpeed, ent>Type, ent->Characteristics->position.toString() */);
			}

			++entity;
		}

		Sleep(1000);
	}

	DEBUG_PRINT("EntityMapCleaner ended!\n");
}