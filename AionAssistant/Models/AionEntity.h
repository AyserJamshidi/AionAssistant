#pragma once
#include "Structures.h"
#include <stdint.h>

enum EntityName {
	LivingGameEntity, // 0
	unk1, // 1
	unk2, // 2
	unk3, // 3
	unk4, // 4
	unk5, // 5
	unk6, // 6
	unk7, // 7
	unk8, // 8
	unk9, // 9
	unk10, // 10
	unk11, // 11
	unk12, // 12
	unk13, // 13
	unk14, // 14
	unk15, // 15
	unk16, // 16
	PlaceableObject, // 17
	unk18, //PlaceableObject, // 18
	unk19, // 19
	unk20, // 20
};

class AionEntity {
public:
	char pad_0x0000[0x198];
	LivingEntityClass* CLivingEntity; // 0x198
	char pad_0x01A0[0x28];
	uint64_t entityType; //0x1C8
	char pad_0x01D0[0x8];
	char entityTypeName[16]; // 0x1D8
	char pad_0x01E8[0x198];
	EntityClass* CEntity; // 0x380

private:

};