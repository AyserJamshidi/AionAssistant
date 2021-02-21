#include <iostream>
#include <thread>

#include <math.h>

#include "../CodeCave/detour.h"
#include "../UberSwitch/include/uberswitch.hpp"
#include "../Models/AionEntity.h"
#include "entity_handler.h"
#include "../hook_handler.h"
#include "../test_thing.h"

extern "C" {
	uintptr_t jmpBackAddr;

	void EntityIntercept(AionEntity *givenAddress);
	void AION();
}
std::vector<AionEntity *> getDistanceOf;
AionEntity **ourPlayer;
bool addToArray = true;

// Address Information
int byteLength = 16; // Amount of bytes we're going to "eat"
uintptr_t starting_address;

// Header
void EntitySleeper(int sleepTime);

void EntityHandler::Initialize(uintptr_t addressOffset) {
	uintptr_t entityHandle = (uintptr_t)GetModuleHandleW(L"CryEntitySystem.dll");

	if (entityHandle != 0) {
		starting_address = entityHandle + addressOffset; // 0x5A107;

		jmpBackAddr = starting_address + byteLength; // CryEntitySystem.dll + 0x5A107 + 16 bytes

		DetourFunction((LPVOID)(starting_address), (LPVOID)AION, byteLength);
	} else {
		printf("Could not find CryEntitySystem.dll base address.");
	}
}

void EntityHandler::Uninitialize() {
	DetourRemove((LPVOID)(starting_address), byteLength);
	starting_address = 0;
	addToArray = false;
	entity_container.clear();
}


void EntityIntercept(AionEntity *givenAddress) {
	if (addToArray) {
		if (givenAddress->CEntity != nullptr) {
			EntityClass *currentEntity = givenAddress->CEntity;
			char firstLetter = currentEntity->name[0];

			if (firstLetter != '\0' && currentEntity->serverEntityId != 0) {
				uberswitch((std::wstring)givenAddress->CEntity->name) {
					case (L"Abyss Deep Layer Fragment"):
					case (L"Shifting Piece of Rock"):
					case (L"Suspicious Bush"):
					case (L"Sealed Ancient Relic"):
						//case (L"Gloriad's Secret Brazier"):
						//case (L"Detachment Commander"):
							//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thingy::OutputTester, givenAddress, 0, 0));
						getDistanceOf.push_back(givenAddress);
						break;
					default:
						break;
				}
			}

			if (((std::string)givenAddress->entityTypeName).compare("Player") == 0) {
				thingy::SetPlayer(givenAddress);
				addToArray = false;
				for (AionEntity *currentEntity : getDistanceOf) {
					//wprintf(L"Name: %s\n", currentEntity->CEntity->name);

					CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thingy::OutputTester, currentEntity, 0, 0));
					//printf("%f", currentEntity->CLivingEntity);


					//wprintf(L"%s - %i -  ID: %u\n", currentEntity->CEntity->name, distance, currentEntity->CEntity->serverEntityId);
					//wprintf(L"Hi! == %p \n", currentEntity->CLivingEntity);
					//wprintf(L"Hi! == %p \n", (float) currentEntity->CLivingEntity->position.x);
				}

				std::thread(EntitySleeper, 200).detach();
				return;
			} else
				entity_container.push_back(givenAddress);
		}
		//}
	}
}

void EntitySleeper(int sleepTime) {
	//printf("Sleeping...\n");
	Sleep(sleepTime);
	entity_container.clear();
	getDistanceOf.clear();
	system("CLS");
	addToArray = true;
}