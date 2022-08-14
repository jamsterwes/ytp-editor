#pragma once
#include "glui.h"
#include <string>

namespace glui {
	class GLUI_INTERFACE ITextRenderer {
	public:
		virtual void add(char c, types::vec2 pos, float size, types::color color) = 0;
		virtual void add(std::string text, types::vec2 pos, float size, types::color color) = 0;
		virtual void draw() = 0;
	};
}