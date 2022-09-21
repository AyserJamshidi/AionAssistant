#include "../../include/thread_managers/threads.hpp"
#include "../../../Dependencies/traypp/include/tray.hpp"
#include "../structures/internalstructures.hpp"
#include <windows.h>
#include <tchar.h>
#include <iostream>

bool startThing = 0;

void testfunc(bool test) {
	printf("Hello from testfunc() == %i!\n", startThing);
}

void testy(GlobalVars globalVars) {
	printf("Inside testy!\n");
	Tray::Tray tray("My Tray", "");

	tray.addEntries(
		Tray::Label("AionAssistant"),
		Tray::Separator(),
		Tray::Toggle("Text", startThing, testfunc),
		Tray::Button("Exit", [&] { tray.exit(); })
	);

	tray.run(globalVars.isRunning);
	printf("Ended testy!\n");
}

// API: https://github.com/zserge/tray
void AionAssistantThread::TrayManager(void* givenStruct) {
	GlobalVars globalVars = *(GlobalVars*)givenStruct;

	printf("TrayManager started!\n");

	/*CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)testy, &globalVars, 0, 0));

	while (*globalVars.isRunning) {
		Sleep(1000);
	}

	tray.exit();*/

	printf("TrayManager ended!\n");
}