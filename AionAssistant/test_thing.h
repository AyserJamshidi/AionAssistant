#pragma once
#include "Models/AionEntity.h"

#include <vector>

const wchar_t *hidden_mobs[] {
	L"Abyss Deep Layer Fragment",
	L"Shifting Piece of Rock",
	L"Suspicious Bush",
	L"Sealed Ancient Relic",
	L"Gloriad's Secret Brazier" // Maybe change this later?
};

class thingy {

private:

public:
	static void OutputTester(AionEntity *entity);
	static void thingy::SetPlayer(AionEntity *entity);
};