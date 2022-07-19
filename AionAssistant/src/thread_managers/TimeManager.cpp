#include <stdio.h>

#include "threads.hpp"
#include "../structures/internalstructures.hpp"

void AionThreads::TimeUpdater(void* givenStruct) {
	GlobalNeeds globalNeeds = *(GlobalNeeds*)givenStruct;

	while (*globalNeeds.isRunning) {
		(*globalNeeds.currentTime)++;
		printf("Time is now %i\n", (*globalNeeds.currentTime));
		Sleep(1000);
	}

	printf("TimeUpdater ended!\n");
}