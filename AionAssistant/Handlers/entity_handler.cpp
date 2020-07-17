#include <iostream>
#include <vector>
#include <thread>

#include "entity_handler.h"
#include "../CodeCave/detour.h"
#include "../Models/AionEntity.h"

extern "C" {
	uintptr_t jmpBackAddr;

	void EntityIntercept(AionEntity* givenAddress);
	void AION();
}

std::vector<AionEntity*> entityVect;
bool addToArray = true;

// Address Information
int byteLength = 16;
uintptr_t initialAddr;

// Header
void EntitySleeper(int sleepTime);

void EntityHandler::Initialize(uintptr_t addressOffset) {
	uintptr_t entityHandle = (uintptr_t)GetModuleHandleW(L"CryEntitySystem.dll");

	if (entityHandle != 0) {
		initialAddr = entityHandle + addressOffset; // 0x5A107;

		jmpBackAddr = initialAddr + byteLength;

		DetourFunction((LPVOID)(initialAddr), (LPVOID)AION, byteLength);
	}
}

void EntityHandler::Uninitialize() {
	DetourRemove((LPVOID)(initialAddr), byteLength);
	initialAddr = 0;
	addToArray = true;
}

void EntityIntercept(AionEntity* givenAddress) {

	if (addToArray) {
		// Test 1: Works only when we're not moving.  Too unreliable.  Bad choice.
		//if (!testVect.empty() && testVect.front() == givenAddress) {

		// Test 2: Traverses the entire container even if we already found 1.  Inefficient.
		//if (std::count(testVect.begin(), testVect.end(), givenAddress)) {

		// Test 3: This works similarly to any_of except this returns the value, which we don't want.
		// Is there anything like this that returns bool?
		if (givenAddress->entityType == LivingGameEntity) {
			if (std::find(entityVect.begin(), entityVect.end(), givenAddress) != entityVect.end()) {
				addToArray = false;
				std::thread(EntitySleeper, 1000).detach();
				return;
			}

			printf("%I64x - type: %s\n", givenAddress, givenAddress->entityTypeName);
			entityVect.push_back(givenAddress);
		}
	}
}

void EntitySleeper(int sleepTime) {
	printf("Sleeping...\n");
	Sleep(sleepTime);
	//std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
	entityVect.clear();
	system("CLS");
	addToArray = true;
}