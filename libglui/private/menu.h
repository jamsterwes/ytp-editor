#pragma once
#include "public/glui.h"
#include "private/window.h"
#include <unordered_map>
#include <Windows.h>

namespace glui {

	class Menu : public IMenu {
	public:
		Menu(Window* window);
		~Menu();

		// Overrides
		void item(std::string label, GLUIMenuItemCallback cb) override;
		IMenu* submenu(std::string label) override;
		void separator() override;
		void refresh() override;

		// Internals
		HMENU getHandle();
	private:
		Window* _window;
		HMENU _menu;
		std::vector<IMenu*> _submenus;
	};

}