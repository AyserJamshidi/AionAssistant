#pragma once
#include <Windows.h>
#include <ctime>

class AionThreads {
public:
	static void TimeUpdater(void* givenStruct);
	static void EntityMapManager(void* givenStruct);
};