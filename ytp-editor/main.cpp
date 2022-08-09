#include "glui.h"
#include <Windows.h>

void saveCallback(glui::IWindow* window) {
	MessageBoxA(nullptr, "Saving file!", "ytp-editor v0.1a", MB_OK | MB_ICONINFORMATION);
}

void debugCallback(glui::IWindow* window) {
	window->toggleConsole();
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	glui::IContext* ctx = glui::init();

	glui::IWindow* win = ctx->makeWindow("ytp-editor v0.1a", 800, 600);
	win->setShortcutCallback(saveCallback, 'S', glui::mods::CTRL);
	win->setShortcutCallback(debugCallback, 0xC0, glui::mods::CTRL);
	win->setBackgroundColor({0.25, 0.75, 1, 1});

	glui::IWindow* win2 = win->makeSubwindow("Shader Editor", 400, 400);
	win2->setBackgroundColor({ 0, 0, 0, 1 });

	while (win->isRunning()) {
		ctx->pollEvents();
		win->render();
		win2->render();
	}
}