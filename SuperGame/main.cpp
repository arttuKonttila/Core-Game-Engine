#include "TheApp.h"

int APIENTRY WinMain(HINSTANCE inst, HINSTANCE prevInst, LPSTR cmdLine, int cmdSHow) {
	auto app = std::make_unique<TheApp>();
	if (!app->Create(1280, 720, "SUPERGAME")) {
		IApplication::Debug("APP START FAILED");
		return 0;
	}
	app->Run();
	return 0;
}