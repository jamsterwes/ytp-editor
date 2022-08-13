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

	enum class keys : unsigned int {
		A = 'A',
		B = 'B',
		C = 'C',
		D = 'D',
		E = 'E',
		F = 'F',
		G = 'G',
		H = 'H',
		I = 'I',
		J = 'J',
		K = 'K',
		L = 'L',
		M = 'M',
		N = 'N',
		O = 'O',
		P = 'P',
		Q = 'Q',
		R = 'R',
		S = 'S',
		T = 'T',
		U = 'U',
		V = 'V',
		W = 'W',
		X = 'X',
		Y = 'Y',
		Z = 'Z'
	};

	class GLUI_INTERFACE IWindow {
	public:
		// Menus
		virtual class IMenu* getMenu() = 0;

		// Subwindow(s)
		virtual IWindow* newModal(std::string title, int width, int height) = 0;

		// Callbacks
		virtual void setKeyCallback(GLUIKeyCallback cb) = 0;
		virtual void setShortcutCallback(GLUIShortcutCallback cb, unsigned int vk, mods mod) = 0;
		virtual void setShortcutCallback(GLUIShortcutCallback cb, unsigned int vk, unsigned int mod) = 0;

		// Debug/state
		virtual bool isRunning() = 0;
		virtual void toggleConsole() = 0;

		// Rendering
		virtual void setBackgroundColor(types::color col) = 0;
		virtual void render() = 0;
		virtual class IUILayer* addLayer(int zIndex) = 0;
	};
}