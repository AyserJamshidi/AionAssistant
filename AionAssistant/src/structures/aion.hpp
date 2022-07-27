#pragma once
#include <cstdint>
#include <string>
#include <format>

// DEFINE code and entity structure from https://guidedhacking.com/threads/auto-padding-for-class-recreation.13478/
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_0(type, name, offset) type name
#define DEFINE_MEMBER_N(type, name, offset) struct { unsigned char MAKE_PAD(offset); type name; }


/*char* entityType[3] = {
	"PLAYER",
	"USER",
	"NPC"
};*/

// This PC Player == "Player"
// Other PC Player == "User"
// NPC == "NPC"
// 

struct PositionVector {
	float x, y, z;
};

class CEntityObject {
public:
	union {
		DEFINE_MEMBER_N(int, ID_Probably, 0x30);
		DEFINE_MEMBER_N(unsigned char, Level, 0x42);
		DEFINE_MEMBER_N(wchar_t, Name[65], 0x46);
		DEFINE_MEMBER_N(wchar_t, Owner[26], 0xCC);
		DEFINE_MEMBER_N(wchar_t, Legion_Name[44], 0x110);
		DEFINE_MEMBER_N(wchar_t, Note[128], 0x68C);

		DEFINE_MEMBER_N(unsigned short int, AnimationSpeed, 0x4FA);
		DEFINE_MEMBER_N(unsigned int, GravityStatus, 0x8D8);

		DEFINE_MEMBER_N(unsigned char, HP_Percent, 0x44); 
	};

};

class CProperties {
public:
	union {
		DEFINE_MEMBER_N(PositionVector, Position, 0x98);
	};
};

class CEntity {
public:
	union {
		DEFINE_MEMBER_0(unsigned short int, IsReal, 0x0);
		DEFINE_MEMBER_N(char, Type[16], 0x1D0);
		DEFINE_MEMBER_N(CProperties*, EntityProperties, 0x180);
		DEFINE_MEMBER_N(CEntityObject*, EntityObject, 0x368);
	};

	void SetName(wchar_t value) {
		*EntityObject->Name = value;
	}
	
	void SetAnimationSpeed(unsigned short int value) {
		EntityObject->AnimationSpeed = value;
	}

	bool IsValidEntity() {
		if (EntityObject == 0)
			return false;

		if (IsReal != 49800)
			return false;

		return true;
	}

	bool IsValidEntityObject() {
		return EntityObject->Name[0] != '\0';
	}
};