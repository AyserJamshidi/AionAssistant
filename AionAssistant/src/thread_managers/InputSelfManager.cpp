#include "../../include/thread_managers/threads.hpp"
#include "../structures/internalstructures.hpp"
#include "../memory/entity/AionEntity.h"
#include "../helpers/helpers.hpp"

#include <stdio.h>

void AionAssistantThread::InputSelfManager(void* givenStruct) {
	HotkeyStructure hotkeyStructure = *(HotkeyStructure*)givenStruct;
	printf("SelfHotkeyManager started!\n");
	HWND hwnd = GetConsoleWindow() == 0 ? Helpers::GetMainHWND() : 0;

	float z = 0;

	CEntity* playerEntity;
	while (*hotkeyStructure.globalVars->isRunning) {
		if ((hwnd != 0 && hwnd != GetForegroundWindow()) // Aion window is not in focus
			|| (*hotkeyStructure.playerAddress) == 0) { // Player address is not set
			Sleep(250);
			continue;
		} 

		// Set the player address
		playerEntity = (CEntity*)(*hotkeyStructure.playerAddress);

		if (!playerEntity->IsValidEntity() // Entity is invalid
			|| !playerEntity->IsValidEntityObject()) { // Entity is not an interactable Entity
			Sleep(250);
			continue;
		}

		printf("Scanning hotkey...\n");


		if (GetAsyncKeyState(VK_INSERT))
			AionEntity::SetAnimationSpeed(playerEntity, 700);

		// Hotkeys
		/*if (GetAsyncKeyState(VK_LSHIFT))
			AionEntity::ModifyPositionBy(playerEntity, 0, 0, z);

		if (GetAsyncKeyState(VK_RSHIFT))
			z = 0;

		if (GetAsyncKeyState(VK_LMENU))
			z -= .1;

		if (GetAsyncKeyState(VK_LCONTROL))
			z += .1;

		if (GetAsyncKeyState(VK_RMENU))
			z--;

		if (GetAsyncKeyState(VK_RCONTROL))
			z++; 

		if (GetAsyncKeyState(VK_F1))
			AionEntity::NoGravity(playerEntity, true);*/
	
		Sleep(200);
	}

	printf("SelfHotkeyManager ended!\n");
}