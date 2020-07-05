#pragma once
#include "Structures.h"

class AionEntity {
public:
	char pad_0x0000[0x198];
	LivingEntityClass* CLivingEntity; // 0x198
	char pad_0x01A0[0x38];
	char entityType[16]; // 0x1D8
	char pad_0x01E8[0x198];
	EntityClass* CEntity; // 0x380

private:

};