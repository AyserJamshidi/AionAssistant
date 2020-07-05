#pragma once
#include <Windows.h>

void* DetourFunction(void* pSource, void* pDestination, int dwLen);
bool DetourRemove(void* pSource, int dwLen);