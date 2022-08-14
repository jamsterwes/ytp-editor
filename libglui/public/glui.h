#pragma once

#define WIN32_LEAN_AND_MEAN

#if defined(LIBGLUI_EXPORTS)
#define GLUI_API  __declspec(dllexport)
#else
#define GLUI_API  __declspec(dllimport)
#endif

#define GLUI_INTERFACE __declspec(novtable)

#include <string>
#include <initializer_list>
#include "types.h"
#include "window.h"
#include "menu.h"
#include "quadrenderer.h"
#include "textrenderer.h"
#include "uilayer.h"

namespace glui {
	class GLUI_INTERFACE IContext {
	public:
		virtual IWindow* newWindow(std::string title, int width, int height) = 0;
		virtual void pollEvents() = 0;

		// Win32 wrappers
		virtual std::string openFileDialog(std::initializer_list<std::pair<std::string, std::string>> types) = 0;
	};

	GLUI_API IContext* init();
	GLUI_API void destroy(IContext* ctx);

	namespace color {
		GLUI_API types::color fromHex(const char* code);
	}
}