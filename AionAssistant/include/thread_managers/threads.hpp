#pragma once
#include <Windows.h>
#include <ctime>

class AionAssistantThread {
public:
	// 
	static void TimeUpdater(void* givenStruct);
	static void EntityMapManager(void* givenStruct);

	// 
	static void LuaVirtualMachineManager(void* givenStruct);

	// Keyboard handlers
	static void InputManager();
	static void InputSelfManager(void* givenStruct);
};