#pragma once
#include "../../structures/aion.hpp"

class AionEntity {
public:
	static void SetAnimationSpeed(CEntity* entity, unsigned short int value);
	static void SetName(CEntity* entity, wchar_t value[65]);
	static PositionVector GetPosition(CEntity* entity);
	static void ModifyPositionBy(CEntity* entity, float valueX, float valueY, float valueZ);
	static void SetPosition(CEntity* entity, PositionVector value);
	static void TraversePosition(CEntity* entity, PositionVector value[]);
	static void NoGravity(CEntity* entity, bool value);
	static float GetDistance(CEntity* entityOne, CEntity* entityTwo);
};