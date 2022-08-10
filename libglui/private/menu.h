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

		// Adders
		void add(MenuItemData item) override;
		void add(std::initializer_list<MenuItemData> items) override;

		// Editors
		void disabled(int id, bool disabled) override;

		// Refreshers
		void refresh() override;

		// Internals
		HMENU getHandle();
	private:
		// Helpers
		void item(std::string label, GLUIMenuItemCallback cb);
		void items(std::initializer_list<std::pair<std::string, GLUIMenuItemCallback>> items);
		IMenu* submenu(std::string label);
		void separator();

		// Data
		Window* _window;
		HMENU _menu;
		std::vector<IMenu*> _submenus;
		std::unordered_map<int, uint64_t> _userToWindows;
		std::unordered_map<int, IMenu*> _userToMenu;
	};

}