#include "private/menu.h"

using namespace glui;

static uint64_t nextMenuID = 1;

Menu::Menu(Window* window) : _window(window), _submenus() {
	_menu = CreateMenu();
}

Menu::~Menu() {
	// Delete all submenus
	for (auto* submenu : _submenus) {
		delete submenu;
	}

	// Delete this menu
	DestroyMenu(_menu);
}

void Menu::item(std::string label, GLUIMenuItemCallback cb) {
	// Get next id
	uint64_t id = nextMenuID++;

	// Register menu callback
	_window->registerCallback(id, cb);

	// Insert menu item
	AppendMenu(_menu, MF_STRING, id, label.c_str());
}

IMenu* Menu::submenu(std::string label) {
	// Create submenu
	Menu* sub = new Menu(_window);

	// Insert submenu
	_submenus.push_back(sub);
	
	// Insert menu item
	AppendMenu(_menu, MF_POPUP, (UINT_PTR)sub->_menu, label.c_str());

	// Return new menu
	return sub;
}

void Menu::separator() {
	// Insert menu separator
	AppendMenu(_menu, MF_SEPARATOR, 0, nullptr);
}

void Menu::refresh() {
	// Refresh menu
	_window->refreshMenu();
}

HMENU Menu::getHandle() {
	return _menu;
}