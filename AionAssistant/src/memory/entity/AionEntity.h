#pragma once
#include "../../structures/aion.hpp"

class AionEntity {
public:
	// Getters
	static PositionVector GetPosition(CEntity* entity);
	static unsigned short GetAnimationSpeed(CEntity* entity);

	// Setters
	static void SetAnimationSpeed(CEntity* entity, unsigned short int value);
	static void SetName(CEntity* entity, wchar_t value[65]);
	static void ModifyPositionBy(CEntity* entity, float valueX, float valueY, float valueZ);
	static void SetPosition(CEntity* entity, float valueX, float valueY, float valueZ);
	static void SetPosition(CEntity* entity, PositionVector value);
	static void TraversePosition(CEntity* entity, PositionVector value[]);
	static void SetNoGravity(CEntity* entity, bool value);
	static float GetDistance(CEntity* entityOne, CEntity* entityTwo);
};