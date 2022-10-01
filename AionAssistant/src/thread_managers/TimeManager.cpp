#include "../../include/thread_managers/threads.hpp"
#include "../structures/internalstructures.hpp"

#include <stdio.h>
#include "../../../Generics/helpers.hpp"

void AionAssistantThread::TimeUpdater(void* givenStruct) {
	GlobalVars globalVars = *(GlobalVars*)givenStruct;
	DEBUG_PRINT("TimeManager started!\n");

	while (*globalVars.isRunning) {
		(*globalVars.currentTime)++;
		Sleep(100);
	}


	DEBUG_PRINT("TimeManager ended!\n");
} 