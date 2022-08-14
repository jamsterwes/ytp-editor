#include "glhelpers.h"

#include <Windows.h>

#include <gl/glew.h>
#include <gl/wglew.h>

#include <cstdio>
#include <string>

using namespace glui::glhelpers;

bool extensionsLoaded = false;

void glui::glhelpers::loadExtensions() {
	// Don't run this twice
	if (extensionsLoaded) return;

	// ** Create fake window
	HRESULT hr = S_OK;
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);

	// ** Create window
	HWND win = CreateWindowEx(
		NULL,
		"GLUI_WINDOW",
		"0",
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		100,
		100,
		NULL,
		NULL,
		hInstance,
		nullptr
	);

	HDC dc = GetDC(win);

	// ** Configure pixel format descriptor

	PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	int idx = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, idx, &pfd);

	// ** Make context
	HGLRC rc = wglCreateContext(dc);

	// ** Set context current
	wglMakeCurrent(dc, rc);

	// ** Load GL/WGL extensions
	glewInit();
	wglewInit();

	// ** Destroy context
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(rc);

	// ** Destroy window
	ReleaseDC(win, dc);
	DestroyWindow(win);

	// ** We are finished
	extensionsLoaded = true;
}

HGLRC glui::glhelpers::createGLContext(HDC dc, int depthBits, int stencilBits) {
	
	loadExtensions();

	int attributeListInt[19];
	int pixelFormat[1];
	int attributeList[7];
	unsigned int formatCount;
	int result;

	// ** Configure pixel format descriptor

	PIXELFORMATDESCRIPTOR pfd = {
	sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,                     // no alpha buffer
		0,                     // shift bit ignored
		0,                     // no accumulation buffer
		0, 0, 0, 0,            // accum bits ignored
		depthBits,
		stencilBits,
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored  
	};

	// ** Set attribute list for OpenGL

	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;

	// Support for rendering to a window.
	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;

	// Support for hardware acceleration.
	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

	// Support for 24bit color.
	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 24;

	// Support for 24 bit depth buffer.
	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = depthBits;

	// Support for double buffer.
	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;

	// Support for swapping front and back buffer.
	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;

	// Support for the RGBA pixel type.
	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;

	// Support for a 8 bit stencil buffer.
	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = stencilBits;

	// Null terminate the attribute list.
	attributeListInt[18] = 0;

	// ** Choose pixel format

	// Query for pixel format matching attributes
	result = wglChoosePixelFormatARB(dc, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if (result != 1) {
		printf("wglChoosePixelFormatARB Error: 0x%X\n", GetLastError());
		return nullptr;
	}

	// Set pixel format
	result = SetPixelFormat(dc, pixelFormat[0], &pfd);
	if (result != 1) {
		printf("SetPixelFormat Error: 0x%X\n", GetLastError());
		return nullptr;
	}

	// ** Select OpenGL version and create context

	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 4;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 6;
	attributeList[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
	attributeList[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
	attributeList[6] = 0;  // null-terminate list

	HGLRC rc;

	while ((rc = wglCreateContextAttribsARB(dc, 0, attributeList)) == NULL) {
		if (--attributeList[3] < 0) {
			attributeList[3] = 9;
			if (--attributeList[1] < 0) {
				attributeList[1] = 1; // Major version (e.g. 4)
				attributeList[3] = 1; // Minor version (e.g. 6) (making it OpenGL v4.6)
			}
		}
		if (attributeList[1] == 1 && attributeList[3] == 0) {
			return nullptr;
		}
	}

	return rc;
}