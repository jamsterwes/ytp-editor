#pragma once
#include "public/glui.h"

namespace glui {
	class GLUI_INTERFACE IDrawable {
	public:
		virtual void draw() = 0;
	};
}