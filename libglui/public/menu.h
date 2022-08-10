#pragma once
#include "glui.h"
#include <vector>

namespace glui {

	typedef void (*GLUIMenuItemCallback)(class IWindow*);

	struct GLUI_API MenuItemData
	{
		enum class Type {
			Item,
			Submenu,
			Separator
		};

		// Data
		std::string label;
		GLUIMenuItemCallback cb;
		std::vector<MenuItemData> subitems;
		Type type;

		MenuItemData(std::string label, GLUIMenuItemCallback cb);                             // Item
		MenuItemData(std::string label, std::initializer_list<struct MenuItemData> submenu);  // Submenu (data)
		MenuItemData();                                                                       // Separator
	};

	class GLUI_INTERFACE IMenu {
	public:
		// Adders
		virtual void add(MenuItemData item) = 0;
		virtual void add(std::initializer_list<MenuItemData> items) = 0;

		// Editors
		virtual void disabled(int id, bool disabled) = 0;

		// Refreshers
		virtual void refresh() = 0;
	};

	namespace menu {
		const MenuItemData SEPARATOR = MenuItemData();
	}
}