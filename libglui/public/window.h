#pragma once
#include "glui.h"

namespace glui {

	typedef void (*GLUIKeyCallback)(class IWindow*, char);
	typedef void (*GLUIShortcutCallback)(class IWindow*);

	enum class mods : unsigned int {
		ALT = 0x1,
		CTRL = 0x2,
		CTRL_ALT = 0x3,
		SHIFT = 0x4,
		ALT_SHIFT = 0x5,
		CTRL_SHIFT = 0x6,
		CTRL_ALT_SHIFT = 0x7,
		WIN = 0x8,
		ALT_WIN = 0x9,
		CTRL_WIN = 0xA,
		CTRL_ALT_WIN = 0xB,
		SHIFT_WIN = 0xC,
		ALT_SHIFT_WIN = 0xD,
		CTRL_SHIFT_WIN = 0xE,
		CTRL_ALT_SHIFT_WIN = 0xF
	};

	class GLUI_INTERFACE IWindow {
	public:
		virtual IWindow* makeSubwindow(std::string title, int width, int height) = 0;
		virtual void setKeyCallback(GLUIKeyCallback cb) = 0;
		virtual void setShortcutCallback(GLUIShortcutCallback cb, unsigned int vk, mods mod) = 0;
		virtual void setShortcutCallback(GLUIShortcutCallback cb, unsigned int vk, unsigned int mod) = 0;
		virtual bool isRunning() = 0;
		virtual void toggleConsole() = 0;
	};
}