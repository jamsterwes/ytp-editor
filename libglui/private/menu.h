#pragma once
#include "public/glui.h"
#include <unordered_map>
#include <Windows.h>

namespace glui {

	class Menu : public IMenu {
	public:
		Menu();
		~Menu();
		void addItem(std::string label, IMenuItemCallback cb) override;
		void addItem(std::string label, IMenu* item) override;
		void addSeparator() override;
		void* getHandle() override;
		void callback(uint64_t id) override;
	private:
		HMENU _menu;
		std::vector<IMenu*> _submenus;
		std::unordered_map<uint64_t, IMenuItemCallback> _itemCallbacks;
	};

}