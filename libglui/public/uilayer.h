#pragma once
#include "glui.h"

namespace glui {

	class GLUI_INTERFACE IUILayer {
	public:
		virtual class IQuadRenderer* getQuadRenderer() = 0;
		virtual class ITextRenderer* getTextRenderer() = 0;
		virtual void draw() = 0;
		virtual void enable() = 0;
		virtual void disable() = 0;
	};

}