#include "modalwindow.h"

using namespace glui;

LRESULT __stdcall glui::GLUIWndProcA(HWND, UINT, WPARAM, LPARAM);

LRESULT __stdcall glui::GLUIModalWndProcA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    ModalWindow* _this = (ModalWindow*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    switch (msg)
    {
    case WM_CLOSE:
        EnableWindow(_this->_parentWindow, true);
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    default:
        return glui::GLUIWndProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

ModalWindow::ModalWindow(HWND parent, std::string title, int width, int height) : _parentWindow(parent) {
    HRESULT hr = S_OK;
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

    // Create window
    HWND window = CreateWindowExA(
        NULL,
        "GLUI_MODAL",
        title.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width,
        height,
        parent,
        NULL,
        hInstance,
        nullptr
    );

    // Attach this class to window
    SetWindowLongPtrA(window, GWLP_USERDATA, (LONG_PTR)this);

    // Initialize window data
    fromCreated(window);

    // Disable parent
    EnableWindow(_parentWindow, false);

    // Show window
    ShowWindow(window, SW_SHOW);
}

ModalWindow::~ModalWindow()
{

}

void ModalWindow::registerWindowClass() {
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

    // Create our window classname configuration
    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpszClassName = "GLUI_MODAL";
    wc.hInstance = hInstance;
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc = GLUIModalWndProcA;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    // Register our classname for the window with our WndProc
    HRESULT hr = RegisterClassExA(&wc);
    if (FAILED(hr)) {
        printf("Failed to register window classname.\nError: 0x%08x\n", hr);
    }
}

void ModalWindow::unregisterWindowClass() {
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
    UnregisterClassA("GLUI_MODAL", hInstance);
}