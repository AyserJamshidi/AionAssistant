#include "../../include/thread_managers/threads.hpp"
#include "../structures/internalstructures.hpp"
#include "../helpers/helpers.hpp"

#include <stdio.h>

void AionAssistantThread::TimeUpdater(void* givenStruct) {
	GlobalVars globalVars = *(GlobalVars*)givenStruct;
	DEBUG_PRINT("TimeManager started!\n");

	while (*globalVars.isRunning) {
		(*globalVars.currentTime)++;
		Sleep(100);
	}

	DEBUG_PRINT("TimeManager ended!\n");
} 