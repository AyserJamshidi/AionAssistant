#pragma once
#include <Windows.h>

BOOL dllTargetIsForeground(int givenProcessID);
HMODULE GetThisDllHandle();

class AttachHelper {
private:

public:
};