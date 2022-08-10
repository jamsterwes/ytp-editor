#pragma once
#include "glui.h"

namespace glui {

	typedef void (*IMenuItemCallback)(class IMenu*);

	class GLUI_INTERFACE IMenu {
	public:
		virtual void addItem(std::string label, IMenuItemCallback cb) = 0;
		virtual void addItem(std::string label, IMenu* item) = 0;
		virtual void addSeparator() = 0;
		virtual void* getHandle() = 0;
		virtual void callback(uint64_t id) = 0;
	};
}