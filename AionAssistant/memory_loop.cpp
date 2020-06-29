#include "memory_loop.h"
#include <vector>
#include <iostream>
#include <Psapi.h>

DWORD gameDllBaseAddress, DlgLootOption, cEntitySystem;

DWORD cEntityOffsets[3] = { 0x58, 0x10, 0x28 };
DWORD cLivingEntityOffset = 0x198;
DWORD playerOffset = 0x380;

//DWORD attackSpeedOffsets[5] = { cEntityOffsets[0], cEntityOffsets[1], cEntityOffsets[2], playerOffset, 0x522 };
std::vector<DWORD> nameOffsets = { cEntityOffsets[0], cEntityOffsets[1], cEntityOffsets[2], playerOffset, 0x46 };
std::vector<DWORD> attackSpeedOffsets = { cEntityOffsets[0], cEntityOffsets[1], cEntityOffsets[2], playerOffset, 0x522 };

MODULEINFO moduleInfo = { 0 };
HMODULE entityHandle = NULL;

HANDLE dllHandle = NULL;

extern "C" void F1();

void entityHook() {

	std::cout << "Hi!" << std::endl;
}

BOOL MemLoop::Initialize(HANDLE givenHandle) {
	dllHandle = givenHandle;

	// Initialize Aion.bin information to find Entity List
	entityHandle = GetModuleHandleA("CryEntitySystem.dll");
	if (entityHandle != NULL)
		GetModuleInformation(GetCurrentProcess(), entityHandle, &moduleInfo, sizeof(MODULEINFO));
	else {
		// TODO: Error message
		return FALSE;
	}

	/*gameDllBaseAddress = (DWORD)GetModuleHandle(L"Game.dll");//dwGetModuleBaseAddress(globalVariables::getProcessID(), L"Game.dll");
	if (gameDllBaseAddress != NULL) {

		DlgLootOption = gameDllBaseAddress + 0x15314E8;
		cEntitySystem = gameDllBaseAddress + 0x15410A8;

		std::cout << "gameDllBase = " << gameDllBaseAddress << std::endl;
	} else {
		// TODO: Error message
		return FALSE;
	}*/

	printf("Initializing...");

	DWORD entityArrAddy = FindPattern("\x48\x8B\x78\x28\x48\x85\xFF", "xxxxxxx");
	printf("entityArrAddy == %x\n", entityArrAddy);
	/*if (entityArrAddy != NULL && entityArrAddy == 0x4B75A0FE) {
		std::cout << "Not null!" << std::endl;
		DWORD EntlistJmpBack = entityArrAddy + 0x7;
		printf("Addr == %x", entityArrAddy);
		PlaceJMP((BYTE*)entityArrAddy, (DWORD)F1);
	} else {
		// TODO: Error message
		printf("Something went wrong!");
	}*/
	return TRUE;
}

void EntityObjStart() {

}

/*__declspec(naked) void entityHook() {
	__asm {
		mov rdi,[rax+28]
		test rdi,rdi
		mov EntityObjStart, esi
		pushad
	}

	_asm {
		mov eax, EntityObjStart
		mov[entsptr], eax
	}
}*/

void PlaceJMP(BYTE* Address, DWORD jumpTo, DWORD length) {
	DWORD dwOldProtect, dwBkup, dwRelAddr;

	//give that address read and write permissions and store the old permissions at oldProtection
	VirtualProtect(Address, length, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	// Calculate the "distance" we're gonna have to jump - the size of the JMP instruction
	dwRelAddr = (DWORD)(jumpTo - (DWORD)Address) - 5;

	// Write the JMP opcode @ our jump position...
	*Address = 0xE9;

	// Write the offset to where we're gonna jump
	//The instruction will then become JMP ff002123 for example
	*((DWORD*)(Address + 0x1)) = dwRelAddr;

	// Overwrite the rest of the bytes with NOPs
	//ensuring no instruction is Half overwritten(To prevent any crashes)
	for (DWORD x = 0x5; x < length; x++)
		*(Address + x) = 0x90;

	// Restore the default permissions
	VirtualProtect(Address, length, dwOldProtect, &dwBkup);
}

DWORD FindPattern(const char* pattern, const char* mask) {
	//Assign our base and module size
	//Having the values right is ESSENTIAL, this makes sure
	//that we don't scan unwanted memory and leading our game to crash
	DWORD base = (DWORD)moduleInfo.lpBaseOfDll;
	DWORD size = (DWORD)moduleInfo.SizeOfImage;

	//Get length for our mask, this will allow us to loop through our array
	DWORD patternLength = (DWORD)strlen(mask);

	for (DWORD i = 0; i < size - patternLength; i++) {
		bool found = true;
		for (DWORD j = 0; j < patternLength; j++) {
			//if we have a ? in our mask then we have true by default, 
			//or if the bytes match then we keep searching until finding it or not
			found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
		}

		//found = true, our entire pattern was found
		//return the memory addy so we can write to it
		if (found) {
			return base + i;
		}
	}

	return NULL;
}

BOOL MemLoop::IsInitialized() {
	return gameDllBaseAddress != NULL;
}

/*DWORD GetPointerAddress(DWORD BaseAddress, std::vector<DWORD> Offsets) {
	DWORD Ptr = *(DWORD*)(BaseAddress);
	DWORD oldPtr;
	if (Ptr == 0) return NULL;

	for (int i = 0; i < Offsets.size(); i++) {
		//if (i == Offsets.size() - 1) {
		if (i == Offsets.size() - 1) {
			oldPtr = Ptr;
			Ptr = (DWORD)(Ptr + Offsets[i]);
			if (Ptr == 0 || oldPtr == Ptr || Ptr == 0xCDCDCDCD) return NULL;
			return Ptr;
		} else {
			oldPtr = Ptr;
			Ptr = *(DWORD*)(Ptr + Offsets[i]);
			if (Ptr == 0 || oldPtr == Ptr || Ptr == 0xCDCDCDCD) return NULL;
		}
	}

	return Ptr;
}*/

template<class CharT = char>
std::basic_string<CharT> read_chars(std::uintptr_t address, std::size_t max_length = 256) {
	std::basic_string<CharT> str(max_length, CharT());
	// I'll assume this throws on failed read.
	// If you are using c++17 you can call str.data() instead of &str[0]
	read(address, &str[0], sizeof(CharT) * max_length);
	// get the position of null terminator
	auto it = str.find(CharT());
	// don't want to waste an allocation and will return the existing string to be reused
	if (it == str.npos)
		str.clear();
	else
		str.resize(it);

	return str;
}

// Internal Memory Reader
uintptr_t GetInternalPtrAddr(uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i) {
		if (addr == NULL || addr == 0xCDCDCDCD)
			return NULL;

		//addr = *(uintptr_t*)addr;
		ReadProcessMemory(dllHandle, (LPVOID)addr, &addr, sizeof(addr), 0);
		//std::cout << "Ptr of addr is " << std::hex << addr << std::endl;

		if (addr == NULL) {
			//std::cout << "Pointer is NULL, we're breaking out! 2" << std::endl;
			return NULL;
		}
		addr += offsets[i];
	}

	//std::cout << "------------Returning address " << std::hex << addr << std::endl;
	return addr;
}

void MemLoop::MainLoop() {

}

/* Make an entity class and input all of this data there instead of having all of this in the mem loop

	- Make sure everything works before adding setters.
*/
char* GetName() {
	//DWORD attackSpeedAddr = GetPointerAddress(cEntitySystem, nameOffsets); //GetInternalPtrAddr(cEntitySystem, attackSpeedOffsets);

	DWORD entityListAddr = GetInternalPtrAddr(cEntitySystem, { 0x58, 0x10, 0x28, 0xE8, 0x30 });
	std::cout << "entityListAddr = " << std::hex << entityListAddr << std::endl;

	if (entityListAddr != NULL) {
		std::cout << "PASSED" << std::endl;

		int x = 1;

		for (DWORD i = 0x0; i < 0x240;) {
			DWORD curEntAddr = GetInternalPtrAddr(entityListAddr, { i });
			std::cout << std::dec << x << " -- " << "curEntAddr == " << std::hex << curEntAddr << " -- ";

			if (curEntAddr != NULL) {
				x++;
				DWORD curNameAddr = GetInternalPtrAddr(curEntAddr, { 0x380, 0x46 });

				if (curNameAddr != NULL) {
					char name[32];

					ReadProcessMemory(dllHandle, (LPVOID)curNameAddr, &name, sizeof(name), NULL);

					int nullCount = 0;
					for (char a : name) {
						if (a == '\00') {
							nullCount += 1;
						} else {
							std::cout << a;
							nullCount = 0;
						}
						if (nullCount >= 2)
							break;
					}
				}
				std::cout << std::endl;
			}
			i += 0x8;
		}
	}

	std::cout << "---------------------------------END" << std::endl;
	return NULL; //name;
}

void GetOwner() {

}

void GetLegion() {

}

void GetNote() {

}

#include <iostream>
float GetX() {
	/*DWORD xPosAddress = FindDMAAddy(0x341D4910, { 0x522 }); //GetPointerAddress(5, playerXOffsets, finalGameAddress);

	if (xPosAddress != NULL) {
		WriteProcessMemory(pHandle, (LPVOID)xPosAddress, &x, sizeof(x), NULL);
	}*/



	/*DWORD ammoAddr = FindDMAAddy(0x341D4910, { 0x522 }); //GetPointerAddress(2, { 0x198, 0xA0 }, NULL);

	int xPos;
	ReadProcessMemory(pHandle, (LPVOID)ammoAddr, &xPos, sizeof(xPos), NULL);
	std::cout << "Num = " << xPos << std::endl;*/
	return 0;// xPos;
}

void GetY() {

}

void GetZ() {

}

void GetCurrentHP() {

}

void GetMaximumHP() {

}

void GetCurrentMP() {

}

void GetMaximumMP() {

}

int GetAttackSpeed() {
	DWORD attackSpeedAddr = NULL; //GetPointerAddress(cEntitySystem, attackSpeedOffsets);

	int atkSpd = NULL;
	if (attackSpeedAddr != NULL) {
		ReadProcessMemory(dllHandle, (LPVOID)attackSpeedAddr, &atkSpd, sizeof(atkSpd), NULL);
	}

	return atkSpd;
}

void GetMovementSpeed() {

}

void GetAttackRange() {

}

void GetDetectionLevel() { // Find a better name for this (Eye hack)

}

void GetTargetOfTarget() { // Make it return the entity addr

}