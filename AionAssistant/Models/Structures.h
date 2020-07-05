#pragma once
#include <Windows.h>

// Used exclusively for getting entity position
class LivingEntityClass {
private:
	struct Vector3 { float x, y, z; };

public:
	char pad_0x0000[0xA0];
	Vector3 position; //0xA0 

};

class EntityClass {
public:
	char pad_0x0000[0x40];
	WORD N0000011B; // 0x40
	char pad_0x0042[0x3];
	char N00000225; // 0x45
	wchar_t name[66]; // 0x46
	WORD N0000027B; 
	wchar_t owner[27]; //0x00CC
};