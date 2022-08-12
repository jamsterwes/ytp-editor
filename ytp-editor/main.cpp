#include "glui.h"
#include <Windows.h>
#include <ctime>

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

void drawStage(glui::IQuadRenderer* renderer) {
	// Draw tab drawer
	float tabDrawerHeight = 35;
	renderer->add(
		{ 336, 663 - tabDrawerHeight },
		{ 930, tabDrawerHeight },
		{ 0.08, 0.08, 0.08, 1 },
		0
	);

	// Draw tab drawer stroke
	renderer->add(
		{ 336 + 1, 663 - tabDrawerHeight + 1 },
		{ 930 - 2, tabDrawerHeight - 2 },
		{ 0.1, 0.1, 0.1, 1 },
		0
	);

	// Draw stage stroke
	renderer->add(
		{ 336, 232 },
		{ 930, 663 - 232 - tabDrawerHeight },
		{ 0.11, 0.11, 0.11, 1 },
		0
	);

	// Draw stage
	renderer->add(
		{ 336 + 1, 232 + 1 },
		{ 930 - 2, 663 - 232 - 2 - tabDrawerHeight },
		{ 0.125, 0.125, 0.125, 1 },
		0
	);

	// Draw stage content
	float stageMargin = 10;
	renderer->add(
		{ 336 + stageMargin, 232 + stageMargin },
		{ 930 - 2 * stageMargin, 663 - 232 - 2 * stageMargin - tabDrawerHeight },
		{ 1, 1, 1, 1 },
		0
	);
}

void drawTimeline(glui::IQuadRenderer* renderer) {
	// Color palette
	glui::types::color trackPalette[6] = {
		glui::color::fromHex("75f07b"),
		glui::color::fromHex("eaf075"),
		glui::color::fromHex("f0bf75"),
		glui::color::fromHex("f07575"),
		glui::color::fromHex("c175f0"),
		glui::color::fromHex("7d75f0")
	};

	// Draw timeline
	renderer->add(
		{ 336, 0 },
		{ 930, 232 },
		{ 0.18, 0.18, 0.18, 1 },
		0
	);

	// Draw video tracks
	int tracks = 6;
	float margin = 1;
	float width = 930 - margin * 2;
	float height = (232 - (margin * (tracks + 1))) / (float)tracks;

	float iconSize = 20;
	
	srand(time(NULL));
	for (int i = 0; i < tracks; i++) {
		// Draw background
		renderer->add(
			{ 336 + margin, margin * (i + 1) + height * i },
			{ width, height },
			{ 0.25, 0.25, 0.25, 1 },
			0
		);

		// Draw video track
		int trackWidth = (rand() / (float)RAND_MAX) * (width - iconSize * 2) + iconSize * 2;
		int trackOffset = (rand() / (float)RAND_MAX) * (width - trackWidth);
		int colorIdx = (int)((rand() / (float)RAND_MAX) * 5);
		renderer->add(
			{ 336 + margin + trackOffset, margin * (i + 1) + height * i},
			{ (float)trackWidth, height },
			trackPalette[colorIdx],
			0
		);

		// Draw track "icon"
		auto colorDark = trackPalette[colorIdx];
		colorDark.x *= 0.5f;
		colorDark.y *= 0.5f;
		colorDark.z *= 0.5f;
		renderer->add(
			{ 336 + margin + trackOffset + (height / 2 - iconSize / 2), margin * (i + 1) + height * i + (height / 2 - iconSize / 2)},
			{ iconSize, iconSize },
			colorDark,
			0
		);
	}
}

void drawFiles(glui::IQuadRenderer* renderer) {
	// Color palette
	auto file = glui::color::fromHex("fff2ab");
	auto fileStroke = glui::color::fromHex("635e43");

	// Draw file browser
	renderer->add(
		{ 0, 0 },
		{ 336, 232 },
		{ 0.2, 0.2, 0.2, 1 },
		0
	);

	// Draw files
	int filesx = 4;
	int filesy = 3;
	glui::types::vec2 margin { 20, 30 };
	glui::types::vec2 tabSize{ 32, 12 };
	float tabDip = 6.0f;
	// Calc width/height
	float width = (336 - margin.x * (filesx + 1)) / filesx;
	float height = (232 - margin.y * (filesy + 1)) / filesy;
	for (int xi = 0; xi < filesx; xi++) {
		for (int yi = 0; yi < filesy; yi++) {
			// Add file stroke
			renderer->add(
				{ margin.x * (xi + 1) + width * xi, margin.y * (yi + 1) + height * yi },
				{ width, height },
				fileStroke,
				0
			);
			// Add file
			renderer->add(
				{ margin.x * (xi + 1) + width * xi + 1, margin.y * (yi + 1) + height * yi + 1 },
				{ width - 2, height - 2 },
				file,
				0
			);
			// Add tab stroke
			renderer->add(
				{ margin.x * (xi + 1) + width * xi, margin.y * (yi + 1) + height * (yi + 1) - tabDip },
				{ tabSize.x, tabSize.y },
				{ 0.345 * 0.8, 0.678 * 0.8, 0.78 * 0.8, 1 },
				0
			);
			// Add tab
			renderer->add(
				{ margin.x * (xi + 1) + width * xi + 1, margin.y * (yi + 1) + height * (yi + 1) - tabDip + 1 },
				{ tabSize.x - 2, tabSize.y - 2 },
				{ 0.345, 0.678, 0.78, 1 },
				0
			);
		}
	}
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

	glui::IWindow* win = ctx->newWindow("ytp-editor v0.1a", 1280, 720);
	win->setShortcutCallback(openCallback, 'O', glui::mods::CTRL);
	win->setShortcutCallback(saveCallback, 'S', glui::mods::CTRL);
	win->setShortcutCallback(debugCallback, 0xC0, glui::mods::CTRL);
	win->setBackgroundColor({ 0.15, 0.15, 0.15, 1 });

	setupMenuBar(win);

	// Draw timeline
	drawTimeline(win->getQuadRenderer());

	// Draw stage
	drawStage(win->getQuadRenderer());

	// Draw files
	drawFiles(win->getQuadRenderer());

	while (win->isRunning()) {
		ctx->pollEvents();
		win->render();
	}
}