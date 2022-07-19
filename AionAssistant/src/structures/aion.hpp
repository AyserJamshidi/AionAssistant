#pragma once
#include <cstdint>
#include <string>
#include <format>

// DEFINE code and entity structure from https://guidedhacking.com/threads/auto-padding-for-class-recreation.13478/
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct { unsigned char MAKE_PAD(offset); type name; }

enum EntityType {
	PLAYER,
	USER,
	NPC
};

char* entityType[3] = {
	"PLAYER",
	"USER",
	"NPC"
};

// This PC Player == "Player"
// Other PC Player == "User"
// NPC == "NPC"
// 



struct Position {
	float x, y, z;

	const char* toString() {
		return std::format("{} {} {}", x, y, z).c_str();
	}
};

class ActualEntity {
public:
	union {
		//DEFINE_MEMBER_N(std::string, EntityType[16], 0x1D0);
		DEFINE_MEMBER_N(int, ID_Probably, 0x30);
		DEFINE_MEMBER_N(byte, Level, 0x42);
		DEFINE_MEMBER_N(wchar_t, Name[66], 0x46);
		DEFINE_MEMBER_N(wchar_t, Owner[34], 0xCC);
		DEFINE_MEMBER_N(wchar_t, Legion_Name[128], 0x110);
		DEFINE_MEMBER_N(wchar_t, Note[128], 0x68C);

		DEFINE_MEMBER_N(unsigned short int, AnimationSpeed, 0x4FA);

		DEFINE_MEMBER_N(byte, HP_Percent, 0x44);
	};

};

class LivingEntity {
public:
	union {
		DEFINE_MEMBER_N(Position, position, 0x98);
	};
};

class AionEntity {
public:
	union {
		DEFINE_MEMBER_N(wchar_t, Type[16], 0x1D0);
		DEFINE_MEMBER_N(LivingEntity*, Characteristics, 0x180);
		DEFINE_MEMBER_N(ActualEntity*, actualEntity, 0x368);
	};

	float GetDistance(AionEntity* entityOne, AionEntity* entityTwo) {
		Position entOne = entityOne->Characteristics->position;
		Position entTwo = entityTwo->Characteristics->position;

		// Distance formula
		return sqrt(pow(entTwo.x - entOne.x, 2) + pow(entTwo.y - entOne.y, 2));
	}
	//char pad_0000[872]; //0x0000
	//class ActualEntity* actualEntity; //0x0368

};