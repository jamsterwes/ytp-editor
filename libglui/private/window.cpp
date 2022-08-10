#include "private/window.h"
#include "private/modalwindow.h"
#include "private/menu.h"
#include <gl/GL.h>

using namespace glui;

LRESULT CALLBACK glui::GLUIWndProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	Window* _this = (Window*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    switch (msg)
    {
    case WM_CLOSE:
        _this->_running = false;
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    case WM_HOTKEY:
        if (GetActiveWindow() != _this->_window) {
            return DefWindowProcA(hwnd, msg, wParam, lParam);
        }
        if (_this->_shortcutCallbacks[wParam]) {
            _this->_shortcutCallbacks[wParam](_this);
        }
        break;
    case WM_KEYDOWN:
        if (_this->_keyCallback) {
            _this->_keyCallback(_this, wParam);
        }
        break;
    case WM_COMMAND:
        if (_this->_menuCallbacks.find(wParam) != _this->_menuCallbacks.end() && _this->_menu) {
            _this->_menuCallbacks[wParam](_this);
        }
        break;
    default:
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

Window::Window(std::string title, int width, int height) : Window() {

    _title = title;
    _width = width;
    _height = height;

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

    // Initialize console
    _initializeConsole();

    // Initialize OpenGL
    _initializeOpenGL();

    // Show window
    ShowWindow(_window, SW_SHOW);
}

Window::Window() :
    _menu(nullptr),
    _title(""),
    _width(0),
    _height(0),
    _running(true),
    _debugConsole(false),
    _clearColor(0, 0, 0, 1),
    _window(nullptr),
    _consoleWindow(nullptr),
    _drawCtx(nullptr),
    _glCtx(nullptr),
    _keyCallback(nullptr),
    _shortcutCallbacks() {
}

Window::~Window() {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(_glCtx);
}

IMenu* Window::getMenu() {
    if (_menu == nullptr) {
        _menu = new Menu(this);
        SetMenu(_window, _menu->getHandle());
    }
    return _menu;
}

void Window::registerCallback(uint64_t id, GLUIMenuItemCallback cb) {
    _menuCallbacks.insert({ id, cb });
}

void Window::refreshMenu() {
    DrawMenuBar(_window);
}

IWindow* Window::newModal(std::string title, int width, int height) {
	return new ModalWindow(_window, title, width, height);
}

bool Window::isRunning() {
    return _running;
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

void Window::toggleConsole() {
    if (_debugConsole) {
        ShowWindow(_consoleWindow, SW_HIDE);
    } else {
        ShowWindow(_consoleWindow, SW_SHOW);
    }
    _debugConsole = !_debugConsole;
}

void Window::setBackgroundColor(types::color col) {
    _clearColor = col;
}

void Window::render() {
    // Make context current
    wglMakeCurrent(_drawCtx, _glCtx);

    // Clear to color
    glClearColor(_clearColor.x, _clearColor.y, _clearColor.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Swap buffers
    wglSwapLayerBuffers(_drawCtx, WGL_SWAP_MAIN_PLANE);
}

void Window::fromCreated(HWND createdWindow) {
    // Set _window
    _window = createdWindow;

    // Get window title
    char buf[256];
    GetWindowText(createdWindow, buf, 256);
    _title = buf;

    // Get window size
    RECT bounds;
    GetWindowRect(createdWindow, &bounds);
    _width = bounds.right - bounds.left;
    _height = bounds.bottom - bounds.top;

    // Initialize OpenGL
    _initializeOpenGL();
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

void Window::unregisterWindowClass() {
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
    UnregisterClassA("GLUI_WINDOW", hInstance);
}

void Window::_initializeConsole() {
    AllocConsole();
    FILE* stream;
    freopen_s(&stream, "CONIN$", "rb", stdin);
    freopen_s(&stream, "CONOUT$", "wb", stdout);
    freopen_s(&stream, "CONOUT$", "wb", stderr);
    
    _consoleWindow = GetConsoleWindow();
    ShowWindow(_consoleWindow, SW_HIDE);
    SetForegroundWindow(_window);
}

void Window::_initializeOpenGL() {
    // Create draw ctx
    _drawCtx = GetDC(_window);

    // TODO: check this for issues
    // Set pixel format for use by OpenGL
    PIXELFORMATDESCRIPTOR pfd = { 
    sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
        1,                     // version number  
        PFD_DRAW_TO_WINDOW |   // support window  
        PFD_SUPPORT_OPENGL |   // support OpenGL  
        PFD_DOUBLEBUFFER,      // double buffered  
        PFD_TYPE_RGBA,         // RGBA type  
        24,                    // 24-bit color depth  
        0, 0, 0, 0, 0, 0,      // color bits ignored  
        0,                     // no alpha buffer  
        0,                     // shift bit ignored  
        0,                     // no accumulation buffer  
        0, 0, 0, 0,            // accum bits ignored  
        32,                    // 32-bit z-buffer  
        0,                     // no stencil buffer  
        0,                     // no auxiliary buffer  
        PFD_MAIN_PLANE,        // main layer  
        0,                     // reserved  
        0, 0, 0                // layer masks ignored  
    };
    int formatIdx = ChoosePixelFormat(_drawCtx, &pfd);
    SetPixelFormat(_drawCtx, formatIdx, &pfd);

    // Create OpenGL ctx
    _glCtx = wglCreateContext(_drawCtx);

    // Make context current
    wglMakeCurrent(_drawCtx, _glCtx);

    // Clear to color
    glClearColor(0.5f, 0.75f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Swap buffers
    wglSwapLayerBuffers(_drawCtx, WGL_SWAP_MAIN_PLANE);
}