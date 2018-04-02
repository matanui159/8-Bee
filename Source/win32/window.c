/*
 * window.c
 *
 * Copyright 2018 Joshua Michael Minter
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../window.h"
#include "../error.h"
#include <windows.h>
#include <stdlib.h>

static const WCHAR g_class_name[] = L"8bee";
static HWND g_window;

static void win32_error() {
	DWORD error = GetLastError();
	char* message;
	if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, 0, (char*)&message, 0, NULL) == 0) {
		bee__error("WIN32: %li (%li)", error, GetLastError());
	} else {
		bee__error("WIN32: %s", message);
		LocalFree(message);
	}
	exit(EXIT_FAILURE);
}

static void class_exit() {
	UnregisterClassW(g_class_name, GetModuleHandleW(NULL));
}

static void window_exit() {
	DestroyWindow(g_window);
}

static LRESULT CALLBACK window_proc(HWND wnd, UINT msg, WPARAM wpm, LPARAM lpm) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(EXIT_SUCCESS);
		break;
	}
	return DefWindowProcW(wnd, msg, wpm, lpm);
}

void bee__window_init() {
	HINSTANCE instance = GetModuleHandleW(NULL);

	WNDCLASSW window_class = {CS_OWNDC};
	window_class.hInstance = instance;
	window_class.lpszClassName = g_class_name;
	window_class.lpfnWndProc = window_proc;
	window_class.hCursor = LoadCursorW(NULL, (PWSTR)IDC_ARROW);

	if (RegisterClassW(&window_class) == 0) {
		win32_error();
	}
	atexit(class_exit);

	RECT rect = {0, 0, 512, 512};
	DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
	AdjustWindowRect(&rect, style, FALSE);

	g_window = CreateWindowW(
			g_class_name,
			L"",
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL, NULL,
			instance,
			NULL
	);

	if (g_window == NULL) {
		win32_error();
	}
	atexit(window_exit);
}

void bee__window_update() {
	MSG msg;
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
		DispatchMessageW(&msg);
		if (msg.message == WM_QUIT) {
			exit(msg.wParam);
		}
	}
}

EGLNativeWindowType bee__window_get() {
	return g_window;
}
