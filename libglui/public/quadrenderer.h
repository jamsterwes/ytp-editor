#pragma once
#include "glui.h"

namespace glui {
	class GLUI_INTERFACE IQuadRenderer {
	public:
		virtual void add(types::vec2 pos, types::vec2 size, types::color color, float borderRadius = 0.0f) = 0;
		virtual void draw() = 0;
	};
}