#include "memoryLoop.h"

DWORD gameDllAddition = 0x15410E8;

DWORD pBaseOffsets[3] = { 58, 10, 28 };
DWORD nameOffsets[5] = { pBaseOffsets[0], pBaseOffsets[1], pBaseOffsets[2], 380, 46 };

DWORD gameDllBaseAddress;

DWORD GetPointerAddress(int PointerLevel, DWORD Offsets[], DWORD baseAddress) { // TODO: Make this with vector class instead of array/pointer level int
	//TODO: Can we just use pHandle as args instead of getting it everytime we are looking for a pointer???  Can improve time.
	DWORD Ptr = *(DWORD*)baseAddress;
	DWORD oldPtr;

	if (Ptr == 0) return NULL;

	for (int i = 0; i < PointerLevel; i++) {
		if (i == PointerLevel - 1) {
			oldPtr = Ptr;
			Ptr = (DWORD)(Ptr + Offsets[i]); // Get Addr + Offset
			if (Ptr == 0 || oldPtr == Ptr) return NULL;
			return Ptr;
		} else {
			oldPtr = Ptr;
			Ptr = *(DWORD*)(Ptr + Offsets[i]); // Get pointer of Addr
			if (Ptr == 0 || oldPtr == Ptr) return NULL;

		}
	}

	return Ptr;
}

void MemLoop::MainLoop() {

}

/* Make an entity class and input all of this data there instead of having all of this shit in the mem loop

	- Make sure everything works before adding setters.
*/

void GetName() {
	DWORD nameAddr = GetPointerAddress(5, nameOffsets, gameDllBaseAddress);
}

void GetOwner() {

}

void GetLegion() {

}

void GetNote() {

}

void GetX() {

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

void GetAttackSpeed() {

}

void GetMovementSpeed() {

}

void GetAttackRange() {

}

void GetDetectionLevel() { // Find a better name for this (Eye hack)

}

void GetTargetsTarget() { // Make it return the entity addr

}