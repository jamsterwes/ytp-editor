#include "glui.h"
#include <Windows.h>

glui::IContext* ctx;

void openCallback(glui::IWindow* window) {
	std::string selected = ctx->openFileDialog({
		{"Video Files", "*.mp4;*.avi;*.mov;*.mkv"},
		{"Audio Files", "*.wav;*.mp3;*.ogg"}
	});
	printf("Selected: '%s'\n", selected.c_str());
}

void saveCallback(glui::IWindow* window) {
	MessageBoxA(nullptr, "Saving file!", "ytp-editor v0.1a", MB_OK | MB_ICONINFORMATION);
}

void debugCallback(glui::IWindow* window) {
	window->toggleConsole();
}

void testingCallback(glui::IMenu* menu) {
	printf("Testing...\n");
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	ctx = glui::init();

	glui::IWindow* win = ctx->newWindow("ytp-editor v0.1a", 800, 600);
	win->setShortcutCallback(openCallback, 'O', glui::mods::CTRL);
	win->setShortcutCallback(saveCallback, 'S', glui::mods::CTRL);
	win->setShortcutCallback(debugCallback, 0xC0, glui::mods::CTRL);
	win->setBackgroundColor({0.25, 0.75, 1, 1});
	
	auto file = win->getMenu()->submenu("&File");
	file->item("&Open\tCtrl+O", openCallback);
	file->item("&Save\tCtrl+S", openCallback);
	file->item("Save &As\tCtrl+Shift+S", openCallback);
	file->refresh();

	while (win->isRunning()) {
		ctx->pollEvents();
		win->render();
	}
}