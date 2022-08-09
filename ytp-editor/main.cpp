#include "glui.h"
#include <Windows.h>

void keyCallback(glui::IWindow* window, char vk) {
	if (vk >= 'A' && vk <= 'Z') {
		printf("Key '%c' pressed!\n", vk);
	} else {
		printf("Key 0x%02x pressed!\n", (unsigned char)vk);
	}
}

void saveCallback(glui::IWindow* window) {
	MessageBoxA(nullptr, "Saving file!", "ytp-editor v0.1a", MB_OK | MB_ICONINFORMATION);
}

void debugCallback(glui::IWindow* window) {
	window->toggleConsole();
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
) {
	glui::IContext* ctx = glui::init();
	glui::IWindow* win = ctx->makeWindow("ytp-editor v0.1a", 800, 600);

	win->setKeyCallback(keyCallback);
	win->setShortcutCallback(saveCallback, 'S', glui::mods::CTRL);
	win->setShortcutCallback(debugCallback, 0xC0, glui::mods::CTRL);

	while (win->isRunning()) {
		ctx->pollEvents();
	}
}