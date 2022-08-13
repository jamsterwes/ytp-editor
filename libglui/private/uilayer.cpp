#include "private/uilayer.h"
#include "private/quadrenderer.h"

using namespace glui;

UILayer::UILayer(int clientWidth, int clientHeight) : _enabled(true) {
	_quadRenderer = new QuadRenderer(clientWidth, clientHeight);
}

UILayer::~UILayer() {
	delete _quadRenderer;
}

IQuadRenderer* UILayer::getQuadRenderer() {
	return _quadRenderer;
}

void UILayer::draw() {
	if (_enabled) {
		_quadRenderer->draw();
	}
}

void UILayer::enable() {
	_enabled = true;
}

void UILayer::disable() {
	_enabled = false;
}