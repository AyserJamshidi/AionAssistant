#pragma once
#include <Windows.h>

// Used exclusively for getting entity position
struct Vector3 {
	float x, y, z;
};

class LivingEntityClass {
private:

public:
	char pad_0x0000[0xA0];
	Vector3 position; //0xA0

};

class EntityClass {
public:
	// We use char instead of byte because a byte is a macro to unsigned char
	char pad_0x0000[0x30]; // Skip 30 bytes
	unsigned int serverEntityId; // Entity ID
	char pad_0x0034[0x10]; // Skip 16 bytes
	unsigned short N0000011B; // 0x40
	wchar_t name[66]; // 0x46
	unsigned short N0000027B;
	wchar_t owner[27]; //0x00CC
};