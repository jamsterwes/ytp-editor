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

void setupMenuBar(glui::IWindow* win) {
	auto menu = win->getMenu();
	menu->add({ "&File", {
		{"&New Project\tCtrl+N", nullptr},
		glui::menu::SEPARATOR,
		{"&Open\tCtrl+O", openCallback},
		glui::menu::SEPARATOR,
		{"&Save\tCtrl+S", nullptr},
		{"Save &As\tCtrl+Shift+S", nullptr},
	} });
	menu->add({ "&Edit", {
		{"&Undo\tCtrl+Z", nullptr},
		glui::menu::SEPARATOR,
		{"Cu&t\tCtrl+X", nullptr},
		{"&Copy\tCtrl+C", nullptr},
		{"&Paste\tCtrl+V", nullptr}
	} });
	menu->refresh();
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	ctx = glui::init();

	glui::IWindow* win = ctx->newWindow("ytp-editor v0.1a", 800, 600);
	win->setShortcutCallback(openCallback, 'O', glui::mods::CTRL);
	win->setShortcutCallback(saveCallback, 'S', glui::mods::CTRL);
	win->setShortcutCallback(debugCallback, 0xC0, glui::mods::CTRL);
	win->setBackgroundColor({ 0.25, 0.75, 1, 1 });

	setupMenuBar(win);

	while (win->isRunning()) {
		ctx->pollEvents();
		win->render();
	}
}