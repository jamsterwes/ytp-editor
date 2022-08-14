#include "private/uilayer.h"
#include "private/quadrenderer.h"
#include "private/textrenderer.h"

using namespace glui;

UILayer::UILayer(int clientWidth, int clientHeight) : _enabled(true) {
	_quadRenderer = new QuadRenderer(clientWidth, clientHeight);
	_textRenderer = new TextRenderer(clientWidth, clientHeight);
}

UILayer::~UILayer() {
	delete _textRenderer;
	delete _quadRenderer;
}

IQuadRenderer* UILayer::getQuadRenderer() {
	return _quadRenderer;
}

ITextRenderer* UILayer::getTextRenderer() {
	return _textRenderer;
}

void UILayer::draw() {
	if (_enabled) {
		_quadRenderer->draw();
		_textRenderer->draw();
	}
}

void UILayer::enable() {
	_enabled = true;
}

void UILayer::disable() {
	_enabled = false;
}