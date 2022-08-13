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


glui::IUILayer* drawStage(glui::IWindow* win) {
	// Get UI layer
	auto stageLayer = win->addLayer(1);
	auto renderer = stageLayer->getQuadRenderer();

	auto videoColor = glui::color::fromHex("99ddff");

	float fakeHeight = 641;

	// Draw tab drawer
	float tabDrawerHeight = 35;
	renderer->add(
		{ 336, fakeHeight - tabDrawerHeight },
		{ 930, tabDrawerHeight },
		{ 0.08, 0.08, 0.08, 1 },
		0
	);

	// Draw tab drawer stroke
	renderer->add(
		{ 336 + 1, fakeHeight - tabDrawerHeight + 1 },
		{ 930 - 2, tabDrawerHeight - 2 },
		{ 0.1, 0.1, 0.1, 1 },
		0
	);

	// Draw stage stroke
	renderer->add(
		{ 336, 232 },
		{ 930, fakeHeight - 232 - tabDrawerHeight },
		{ 0.11, 0.11, 0.11, 1 },
		0
	);

	// Draw stage
	renderer->add(
		{ 336 + 1, 232 + 1 },
		{ 930 - 2, fakeHeight - 232 - 2 - tabDrawerHeight },
		{ 0.125, 0.125, 0.125, 1 },
		0
	);

	// Draw stage content
	float stageMargin = 10;
	renderer->add(
		{ 336 + stageMargin, 232 + stageMargin },
		{ 930 - 2 * stageMargin, fakeHeight - 232 - 2 * stageMargin - tabDrawerHeight },
		videoColor,
		0
	);

	return stageLayer;
}

glui::IUILayer* drawTimeline(glui::IWindow* win) {
	// Get UI layer
	auto timelineLayer = win->addLayer(0);
	auto renderer = timelineLayer->getQuadRenderer();

	// Color palette
	glui::types::color trackPalette[6] = {
		glui::color::fromHex("75f07b"),
		glui::color::fromHex("eaf075"),
		glui::color::fromHex("f0bf75"),
		glui::color::fromHex("f07575"),
		glui::color::fromHex("c175f0"),
		glui::color::fromHex("7d75f0")
	};
	glui::types::color labelPalette[4] = {
		glui::color::fromHex("0088bb"),
		glui::color::fromHex("666666"),
		glui::color::fromHex("666666"),
		glui::color::fromHex("94cbe0"),
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

	float iconSize = 20;
	float labelSize = 40;
	float labelMargin = 2;
	int labelIcons = 3;
	float labelWidth = labelSize * labelIcons + labelMargin * (labelIcons + 1);

	float width = 930 - margin * 2 - labelWidth;
	float height = (232 - (margin * (tracks + 1))) / (float)tracks;
	
	srand(time(NULL));
	for (int i = 0; i < tracks; i++) {

		// Draw "label"
		for (int l = 0; l < labelIcons; l++) {
			renderer->add(
				{ 336 + margin * 2 + labelMargin * (l + 1) + labelSize * l, margin * (i + 1) + height * i + labelMargin},
				{ labelSize - labelMargin * 2, labelSize - labelMargin * 2 },
				labelPalette[i == (tracks - 1) ? l : ((l == 0) ? 3 : l)],
				(l == 0) ? (labelSize - labelMargin * 2) / 2 : 6
			);
		}

		// Draw background
		renderer->add(
			{ 336 + margin + labelWidth, margin * (i + 1) + height * i },
			{ width, height },
			{ 0.25, 0.25, 0.25, 1 },
			0
		);

		// Get dark palette color
		int colorIdx = rand() % 6;
		auto colorDark = trackPalette[colorIdx];
		colorDark.x *= 0.5f;
		colorDark.y *= 0.5f;
		colorDark.z *= 0.5f;

		// Draw video track stroke
		int trackWidth = (rand() / (float)RAND_MAX) * (width - iconSize * 2) + iconSize * 2;
		int trackOffset = (rand() / (float)RAND_MAX) * (width - trackWidth) + labelWidth;
		renderer->add(
			{ 336 + margin + trackOffset, margin * (i + 1) + height * i },
			{ (float)trackWidth, height },
			colorDark,
			6
		);

		// Draw video track
		renderer->add(
			{ 336 + margin + trackOffset + 1, margin * (i + 1) + height * i + 1},
			{ (float)trackWidth - 2, height - 2 },
			trackPalette[colorIdx],
			5
		);

		// Draw track "icon"
		renderer->add(
			{ 336 + margin + trackOffset + (height / 2 - iconSize / 2), margin * (i + 1) + height * i + (height / 2 - iconSize / 2)},
			{ iconSize, iconSize },
			colorDark,
			iconSize / 2
		);
	}

	return timelineLayer;
}

glui::IUILayer* drawFiles(glui::IWindow* win) {
	// Get UI layer
	auto fileLayer = win->addLayer(0);
	auto renderer = fileLayer->getQuadRenderer();

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
	glui::types::vec2 tabSize{ 29, 12 };
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

	return fileLayer;
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
	auto timelineLayer = drawTimeline(win);

	// Draw stage
	auto stageLayer = drawStage(win);

	// Draw files
	auto fileLayer = drawFiles(win);

	while (win->isRunning()) {
		ctx->pollEvents();
		win->render();
	}
}