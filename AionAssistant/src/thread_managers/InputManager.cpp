#pragma comment(lib, "dxguid.lib") // Required for keyboard hooking stuff
#pragma comment(lib, "dinput8.lib") // Required for keyboard hooking stuff

#include "../../include/thread_managers/threads.hpp"
#include "../helpers/helpers.hpp"

#include <dinput.h>

#define DIRECTINPUT_VERSION 0x0800

void* HookVTableFunction(void* pVTable, void* fnHookFunc, int nOffset) {
	intptr_t ptrVtable = *((intptr_t*)pVTable); // Pointer to our chosen vtable
	intptr_t ptrFunction = ptrVtable + sizeof(intptr_t) * nOffset; // The offset to the function (remember it's a zero indexed array with a size of four bytes)
	intptr_t ptrOriginal = *((intptr_t*)ptrFunction); // Save original address

	// Edit the memory protection so we can modify it
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQuery((LPCVOID)ptrFunction, &mbi, sizeof(mbi));
	VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect);

	// Overwrite the old function with our new one
	*((intptr_t*)ptrFunction) = (intptr_t)fnHookFunc;

	// Restore the protection
	VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &mbi.Protect);

	// Return the original function address incase we want to call it
	return (void*)ptrOriginal;
}

bool Hook_DirectInput(bool enable) {
	HINSTANCE hInst = (HINSTANCE)GetModuleHandle(NULL);
	IDirectInput8* pDirectInput = NULL;

	if (DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&pDirectInput, NULL) != DI_OK) {
		DEBUG_PRINT_ERR("Error occurred on keyboard DirectInput8Create\n");
		return -1;
	}

	LPDIRECTINPUTDEVICE8 lpdiKeyboard;
	if (pDirectInput->CreateDevice(GUID_SysKeyboard, &lpdiKeyboard, NULL) != DI_OK) {
		DEBUG_PRINT_ERR("Error occurred on keyboard CreateDevice\n");
		pDirectInput->Release();
		return -1;
	}

	// Set the data type to keyboard
	lpdiKeyboard->SetDataFormat(&c_dfDIKeyboard);

	// We want non-exclusive access i.e sharing it with other applications
	lpdiKeyboard->SetCooperativeLevel(GetActiveWindow(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// Attempt to acquire the device
	if (lpdiKeyboard->Acquire() == DI_OK) {
		DEBUG_PRINT("Acquired keyboard.\n");

		// This is how the data is returned as an array 256 bytes for each key
		BYTE diKeys[256] = { 0 };

		// Start looping and grabbing the data from the device
		while (1) {
			// Poll for new data
			lpdiKeyboard->Poll();
			// Get the state -- https://docs.microsoft.com/en-us/previous-versions/windows/desktop/ee418641(v%3Dvs.85)
			if (lpdiKeyboard->GetDeviceState(256, diKeys) == DI_OK) {
				// Check if the escape was pressed
				if (diKeys[DIK_W] & 0x80) {
					DEBUG_PRINT("W!\n");
					diKeys[DIK_A] = LOBYTE(0x80);
				} else if (diKeys[DIK_Z] & 0x80) {
					break;
				}
			}
			// We don't need realtime access, don't flood the CPU
			Sleep(100);
		}
		// Unacquire the keyboard
		lpdiKeyboard->Unacquire();
	} else
		DEBUG_PRINT_ERR("Failed to acquire keyboard\n");

	// Free the keyboard and device objects
	lpdiKeyboard->Release();
	pDirectInput->Release();

	return true;
}

void AionAssistantThread::InputManager() {
	DEBUG_PRINT("InputManager started!\n");
	Hook_DirectInput(true);
	DEBUG_PRINT("InputManager ended!\n");
}