#include "AionEntity.h"

#include <wchar.h>
#include <iostream>
#include "../../../../Generics/helpers.hpp"

// Getters
PositionVector AionEntity::GetPosition(CEntity* entity) {
	return entity->EntityProperties->Position;
}

unsigned short AionEntity::GetAnimationSpeed(CEntity* entity) {
	return entity->EntityObject->AnimationSpeed;
}

float AionEntity::GetDistance(CEntity* entityOne, CEntity* entityTwo) {
	//PositionVector entOne = entityOne->EntityProperties->position;
	//PositionVector entTwo = entityTwo->EntityProperties->position;

	// Distance formula
	return sqrt(
		powf(entityTwo->EntityProperties->Position.x - entityOne->EntityProperties->Position.x, 2)
		+ powf(entityTwo->EntityProperties->Position.y - entityOne->EntityProperties->Position.y, 2)
	);
}

// Setters
void AionEntity::SetAnimationSpeed(CEntity* entity, unsigned short int value) {
	entity->EntityObject->AnimationSpeed = value;
	DEBUG_PRINT("Set animation!");
}

void AionEntity::SetName(CEntity* entity, wchar_t value[65]) {
	wcsncpy_s(entity->EntityObject->Name, value, 32);
	DEBUG_PRINT("Set name!");
}

void AionEntity::ModifyPositionBy(CEntity* entity, float valueX, float valueY, float valueZ) {
	entity->EntityProperties->Position = {
		entity->EntityProperties->Position.x + valueX,
		entity->EntityProperties->Position.y + valueY,
		entity->EntityProperties->Position.z + valueZ
	};
	DEBUG_PRINT("Modified position!");
}

void AionEntity::TraversePosition(CEntity* entity, PositionVector value[]) {
	/*for (PositionVector curVect : value) {

	}*/
}

// Wrapper
void AionEntity::SetPosition(CEntity* entity, float valueX, float valueY, float valueZ) {
	SetPosition(entity, PositionVector{ valueX, valueY, valueZ });
}

void AionEntity::SetPosition(CEntity* entity, PositionVector value) {
	entity->EntityProperties->Position = value;
	DEBUG_PRINT("Set position!");
}

void AionEntity::SetNoGravity(CEntity* entity, bool value) {
	entity->EntityObject->GravityStatus = (value ? 5 : 0);
}
