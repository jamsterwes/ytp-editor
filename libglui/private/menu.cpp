#include "private/menu.h"

using namespace glui;

static uint64_t nextMenuID = 1;

Menu::Menu(Window* window) : _window(window), _submenus(), _userToWindows(), _userToMenu() {
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

void Menu::add(MenuItemData item) {
	// Call helper function based on item type
	switch (item.type) {
	case MenuItemData::Type::Separator:
		separator();
		break;
	case MenuItemData::Type::Item:
		this->item(item.label, item.cb);
		break;
	case MenuItemData::Type::Submenu:
		{
			IMenu* menu = this->submenu(item.label);
			for (auto item : item.subitems) {
				menu->add(item);
			}
		}
		break;
	}
}

void Menu::add(std::initializer_list<MenuItemData> items) {
	// Add all items
	for (auto item : items) {
		add(item);
	}
}

void Menu::item(std::string label, GLUIMenuItemCallback cb) {
	// Get next id
	uint64_t wid = nextMenuID++;

	// Register id mapping
	_userToWindows.insert({ _userToWindows.size(), wid});

	// Register menu callback
	_window->registerCallback(wid, cb);

	// Insert menu item
	AppendMenu(_menu, MF_STRING, wid, label.c_str());
}


void Menu::items(std::initializer_list<std::pair<std::string, GLUIMenuItemCallback>> items) {
	// Add items
	for (auto& pair : items) {
		if (pair.first == "---") {
			separator();
		} else {
			item(pair.first, pair.second);
		}
	}
}

IMenu* Menu::submenu(std::string label) {
	// Create submenu
	Menu* sub = new Menu(_window);

	// Register id mapping
	_userToMenu.insert({ _userToMenu.size(), sub });

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

// TODO: refactor this
void Menu::disabled(int id, bool disabled) {
	if (_userToWindows.find(id) != _userToWindows.end()) {
		EnableMenuItem(_menu, _userToWindows[id], disabled ? MF_GRAYED : MF_ENABLED);
		refresh();
	}
}

void Menu::refresh() {
	// Refresh menu
	_window->refreshMenu();
}

HMENU Menu::getHandle() {
	return _menu;
}

// Handle constructing menu items by inference

MenuItemData::MenuItemData(std::string label, GLUIMenuItemCallback cb) : cb(cb), label(label), subitems() {
	type = Type::Item;
}

MenuItemData::MenuItemData(std::string label, std::initializer_list<MenuItemData> submenu) : label(label), cb(nullptr), subitems() {
	subitems = std::vector<MenuItemData>(submenu.begin(), submenu.end());
	type = Type::Submenu;
}

MenuItemData::MenuItemData() : label("---"), cb(nullptr), subitems() {
	type = Type::Separator;
}