#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

namespace glui {
	namespace glhelpers{
		void loadExtensions();
		HGLRC createGLContext(HDC dc, int depthBits = 24, int stencilBits = 8);
	}
}