#include "private/window.h"

using namespace glui;

LRESULT CALLBACK glui::GLUIWndProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window* _this = (Window*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CLOSE:
        _this->_running = false;
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    case WM_HOTKEY:
        if (_this->_shortcutCallbacks[wParam] != nullptr) {
            _this->_shortcutCallbacks[wParam](_this);
        }
        break;
    case WM_KEYDOWN:
        if (_this->_keyCallback != nullptr) {
            _this->_keyCallback(_this, wParam);
        }
        break;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

Window::Window(std::string title, int width, int height) : 
    _title(title),
    _width(width),
    _height(height),
    _running(true),
    _debugConsole(false),
    _keyCallback(nullptr),
    _shortcutCallbacks() {
    HRESULT hr = S_OK;
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

    // Create window
    _window = CreateWindowExA(
        NULL,
        "GLUI_WINDOW",
        _title.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        _width,
        _height,
        NULL,
        NULL,
        hInstance,
        nullptr
    );

    // Attach this class to window
    SetWindowLongPtrA(_window, GWLP_USERDATA, (LONG_PTR)this);

    // Show window
    ShowWindow(_window, SW_SHOW);
}

Window::~Window() {

}

IWindow* Window::makeSubwindow(std::string title, int width, int height) {
	return nullptr;
}

void Window::registerWindowClass() {
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

    // Create our window classname configuration
    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpszClassName = "GLUI_WINDOW";
    wc.hInstance = hInstance;
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = GLUIWndProcA;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    // Register our classname for the window with our WndProc
    HRESULT hr = RegisterClassExA(&wc);
    if (FAILED(hr)) {
        printf("Failed to register window classname.\nError: 0x%08x\n", hr);
    }
}

bool Window::isRunning() {
    return _running;
}

void Window::toggleConsole() {
    if (_debugConsole) {
        FreeConsole();
    } else {
        AllocConsole();
        FILE* stream;
        freopen_s(&stream, "CONIN$", "rb", stdin);
        freopen_s(&stream, "CONOUT$", "wb", stdout);
        freopen_s(&stream, "CONOUT$", "wb", stderr);
    }
    _debugConsole = !_debugConsole;
}

void Window::setKeyCallback(GLUIKeyCallback cb) {
    _keyCallback = cb;
}

void Window::setShortcutCallback(GLUIShortcutCallback cb, UINT vk, mods mods) {
    setShortcutCallback(cb, vk, (UINT)mods);
}

void Window::setShortcutCallback(GLUIShortcutCallback cb, UINT vk, UINT mods) {
    // Get ID and modifier flags
    int nextID = _shortcutCallbacks.size() + 1;

    // Register hotkey
    RegisterHotKey(_window, nextID, mods, vk);

    // Insert callback into map
    _shortcutCallbacks.insert({ nextID, cb });
}

void Window::unregisterWindowClass() {
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
    UnregisterClassA("GLUI_WINDOW", hInstance);
}