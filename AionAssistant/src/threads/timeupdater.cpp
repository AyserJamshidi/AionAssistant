#include <stdio.h>

#include "threads.hpp"
#include "../internalstructures.h"

void AionThreads::TimeUpdater(void* givenStruct) {
	TimeStructure timeStruct = *(TimeStructure*)givenStruct;

	while (*timeStruct.globalNeeds->isRunning) {
		(*timeStruct.currentTime)++;
		printf("Time is now %i\n", *(timeStruct.currentTime));
		Sleep(1000);
	}

	printf("TimeUpdater ended!\n");
}