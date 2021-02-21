#include "test_thing.h"

AionEntity* player;
float GetDistance(AionEntity* entityOne, AionEntity* entityTwo);

void thingy::OutputTester(AionEntity* entity) {
	if (player != nullptr) {
		wprintf(L"Name: %s - %f.\n", entity->CEntity->name, GetDistance(player, entity));
	} else {
		printf("Player is null..\n");
	}
}

void thingy::SetPlayer(AionEntity* entity) {
	player = entity;
}

float GetDistance(AionEntity* entityOne, AionEntity* entityTwo) {
	Vector3 entOne = entityOne->CLivingEntity->position;
	Vector3 entTwo = entityTwo->CLivingEntity->position;

	// Distance formula
	return sqrt(pow(entTwo.x - entOne.x, 2) + pow(entTwo.y - entOne.y, 2));
}


/*float xOne = givenAddress->CLivingEntity->position.x;
float yOne = givenAddress->CLivingEntity->position.y;
float xTwo = currentEntity->CLivingEntity->position.x;
float yTwo = currentEntity->CLivingEntity->position.y;
float distance = sqrt(pow(xTwo - xOne, 2) +
	pow(yTwo - yOne, 2));*/