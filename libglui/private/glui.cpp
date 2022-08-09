#include "public/glui.h"
#include "private/window.h"

namespace glui {
	// CLASS

	class Context : public IContext {
	public:
		Context();
		~Context();
		IWindow* makeWindow(std::string title, int width, int height) override;
		void pollEvents() override;
	private:
	};

	// IMPLEMENTATION

	Context::Context() {
		Window::registerWindowClass();
	}

	Context::~Context() {
		Window::unregisterWindowClass();
	}

	IWindow* Context::makeWindow(std::string title, int width, int height) {
		return new Window(title, width, height);
	}

	void Context::pollEvents() {
		MSG msg;
		GetMessage(&msg, nullptr, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// INIT FUNCTION

	GLUI_API IContext* init() {
		return new Context();
	}

	GLUI_API void destroy(IContext* ctx) {
		delete ctx;
	}
}