#pragma once
#include <Windows.h>
#include <ctime>

class AionThreads {
private:

public:
	static void TimeUpdater(void* givenStruct);
	static void EntityMapCleaner(void* givenStruct);
};