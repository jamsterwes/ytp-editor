#include "private/menu.h"

using namespace glui;

static uint64_t nextMenuID = 1;

Menu::Menu() : _submenus(), _itemCallbacks() {
	_menu = CreateMenu();
}

Menu::~Menu() {
	DestroyMenu(_menu);
}

void Menu::addItem(std::string label, IMenuItemCallback cb) {
	// Get next id
	uint64_t id = nextMenuID++;

	// Insert callback
	_itemCallbacks.insert({ id, cb });

	// Insert menu item
	AppendMenu(_menu, MF_STRING, id, label.c_str());
}

void Menu::addItem(std::string label, IMenu* item) {
	// Insert submenu
	_submenus.push_back(item);
	
	// Insert menu item
	AppendMenu(_menu, MF_POPUP, (UINT_PTR)item->getHandle(), label.c_str());
}

void Menu::addSeparator() {
	// Insert menu separator
	AppendMenu(_menu, MF_SEPARATOR, 0, nullptr);
}

void* Menu::getHandle() {
	return _menu;
}

void Menu::callback(uint64_t id) {
	if (_itemCallbacks.find(id) != _itemCallbacks.end()) {
		_itemCallbacks[id](this);
	} else {
		for (auto submenu : _submenus) {
			submenu->callback(id);
		}
	}
}