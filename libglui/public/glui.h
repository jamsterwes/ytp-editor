#pragma once

#define WIN32_LEAN_AND_MEAN

#if defined(LIBGLUI_EXPORTS)
#define GLUI_API  __declspec(dllexport)
#else
#define GLUI_API  __declspec(dllimport)
#endif

#define GLUI_INTERFACE __declspec(novtable)

#include <string>
#include "window.h"

namespace glui {
	class GLUI_INTERFACE IContext {
	public:
		virtual IWindow* makeWindow(std::string title, int width, int height) = 0;
		virtual void pollEvents() = 0;
	};

	GLUI_API IContext* init();
	GLUI_API void destroy(IContext* ctx);
}