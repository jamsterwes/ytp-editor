#pragma once
#include "private/window.h"

namespace glui {

	class ModalWindow : public Window {
	public:
		ModalWindow(HWND parent, std::string title, int width, int height);
		~ModalWindow();

		static void registerWindowClass();
		static void unregisterWindowClass();

		friend LRESULT __stdcall GLUIModalWndProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		// Window Resources
		HWND _parentWindow;
	};
}