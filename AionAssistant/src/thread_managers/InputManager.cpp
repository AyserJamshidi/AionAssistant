#pragma comment(lib, "dxguid.lib") // Required for keyboard hooking stuff
#pragma comment(lib, "dinput8.lib") // Required for keyboard hooking stuff

#include "../../include/thread_managers/threads.hpp"

#include <dinput.h>
#include "../../../Generics/helpers.hpp"

// #define DIRECTINPUT_VERSION 0x0800

// Source: https://guidedhacking.com/threads/how-to-hook-directinput-emulate-key-presses.14011/
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
		return false;
	}

	LPDIRECTINPUTDEVICE8 lpdiKeyboard;
	if (pDirectInput->CreateDevice(GUID_SysKeyboard, &lpdiKeyboard, NULL) != DI_OK) {
		DEBUG_PRINT_ERR("Error occurred on keyboard CreateDevice\n");
		pDirectInput->Release();
		return false;
	}

	lpdiKeyboard->SetDataFormat(&c_dfDIKeyboard);
	lpdiKeyboard->SetCooperativeLevel(GetActiveWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// Setup config for buffered output
	DIPROPDWORD  dipdw = { 0 };
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 10;

	// Attempt to switch to buffered output
	if (lpdiKeyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph) == DI_OK)
		DEBUG_PRINT("Buffered mode enabled for device\n");

	// Trip flag
	bool bEscape = false;

	// Attempt to acquire the device
	if (lpdiKeyboard->Acquire() == DI_OK) {
		DEBUG_PRINT("Acquired keyboard.\n");

		while (true) {
			// Poll for new data
			lpdiKeyboard->Poll();

			// We asked for 10 objects at a time
			DIDEVICEOBJECTDATA didod[10] = { 0 };
			DWORD dwNumElements = 10;

			// Get the data
			HRESULT hr = lpdiKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, &dwNumElements, 0);
			if (hr == DI_OK) {
				// Process the keystrokes
				for (DWORD i = 0; i < dwNumElements; i++) {
					if (LOBYTE(didod[i].dwData) > 0) {
						switch (didod[i].dwOfs) {
							case DIK_W:
								didod[i].dwOfs = DIK_S;
								printf("[W]");
								break;
							case DIK_S:
								printf("[S]");
								break;
							case DIK_A:
								printf("[A]");
								break;
							case DIK_D:
								printf("[D]");
								break;
							case DIK_Z:
								break;
						}
					}
				}
			}

			// Trip flag hit
			if (bEscape)
				break;

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