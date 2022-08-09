#pragma once
#include "public/glui.h"
#include <Windows.h>
#include <unordered_map>

namespace glui {

	class Window : public IWindow {
	public:
		Window();
		Window(std::string title, int width, int height);
		~Window();

		IWindow* makeSubwindow(std::string title, int width, int height) override;
		void setKeyCallback(GLUIKeyCallback cb) override;
		void setShortcutCallback(GLUIShortcutCallback cb, UINT vk, UINT mods) override;
		void setShortcutCallback(GLUIShortcutCallback cb, UINT vk, mods mods) override;
		bool isRunning() override;

		void toggleConsole() override;

		void setBackgroundColor(types::color col) override;
		void render() override;

		void fromCreated(HWND createdWindow);

		static void registerWindowClass();
		static void unregisterWindowClass();

		friend LRESULT __stdcall GLUIWndProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	protected:
		// Window Properties
		std::string _title;
		int _width, _height;

		// Window state
		bool _running;
		bool _debugConsole;
		types::color _clearColor;

		// Window Resources
		HWND _window, _consoleWindow;
		HDC _drawCtx;
		HGLRC _glCtx;

		// Callbacks
		GLUIKeyCallback _keyCallback;
		std::unordered_map<int, GLUIShortcutCallback> _shortcutCallbacks;

		// Initialization helpers
		void _initializeConsole();
		void _initializeOpenGL();
	};
}