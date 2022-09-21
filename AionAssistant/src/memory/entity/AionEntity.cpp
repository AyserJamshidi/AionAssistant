#include "AionEntity.h"

#include <wchar.h>
#include <iostream>

void AionEntity::SetAnimationSpeed(CEntity* entity, unsigned short int value) {
	entity->EntityObject->AnimationSpeed = value;
	printf("set animation!\n");
}

void AionEntity::SetName(CEntity* entity, wchar_t value[65]) {
	wcsncpy_s(entity->EntityObject->Name, value, 32);
	printf("set name!\n");
}

PositionVector AionEntity::GetPosition(CEntity* entity) {
	return entity->EntityProperties->Position;
}

void AionEntity::ModifyPositionBy(CEntity* entity, float valueX, float valueY, float valueZ) {
	entity->EntityProperties->Position = {
		entity->EntityProperties->Position.x + valueX,
		entity->EntityProperties->Position.y + valueY,
		entity->EntityProperties->Position.z + valueZ
	};
	printf("modified position!\n");
}

void AionEntity::SetPosition(CEntity* entity, PositionVector value) {
	entity->EntityProperties->Position = value;
	printf("set position!\n");
}

void AionEntity::TraversePosition(CEntity* entity, PositionVector value[]) {
	/*for (PositionVector curVect : value) {

	}*/
}

void AionEntity::NoGravity(CEntity* entity, bool value) {
	entity->EntityObject->GravityStatus = (value ? 5 : 0);
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