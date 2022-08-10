#pragma once
#include "glui.h"

namespace glui {

	typedef void (*GLUIMenuItemCallback)(class IWindow*);

	class GLUI_INTERFACE IMenu {
	public:
		virtual void item(std::string label, GLUIMenuItemCallback cb) = 0;
		virtual IMenu* submenu(std::string label) = 0;
		virtual void separator() = 0;
		virtual void refresh() = 0;
	};
}