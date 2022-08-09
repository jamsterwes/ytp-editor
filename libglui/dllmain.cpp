#include <Windows.h>
#include "public/glui.h"
#include "private/window.h"

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
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        Window::unregisterWindowClass();
        break;
    }
    return TRUE;
}

