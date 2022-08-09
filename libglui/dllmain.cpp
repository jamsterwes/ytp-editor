#include <Windows.h>
#include "public/glui.h"
#include "private/window.h"
#include "private/modalwindow.h"

using namespace glui;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Window::registerWindowClass();
        ModalWindow::registerWindowClass();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        ModalWindow::unregisterWindowClass();
        Window::unregisterWindowClass();
        break;
    }
    return TRUE;
}

