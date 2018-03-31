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
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

static void win32_error() {
	DWORD error = GetLastError();
	PSTR message;
	if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, 0, (PSTR)&message, 0, NULL) == 0) {
		fprintf(stderr, "%s\n", message);
	} else {
		fprintf(stderr, "%i (%i)", error, GetLastError());
	}
	exit(EXIT_FAILURE);
}

static LRESULT CALLBACK window_proc(HWND, wnd, UINT msg, WPARAM wpm, LPARAM lpm) {
	return DefWindowProcW(wnd, msg, wpm, lpm);
}

EGLNativeWindowType bee__window_create() {
	HINSTANCE instance = GetModuleHandleW(NULL);

	WNDCLASSW window_class = {CS_OWNDC};
	window_class.hInstance = instance;
	window_class.lpszClassName = L"8bee";
	window_class.lpfnWndProc = window_proc;
	window_class.hIcon = LoadIconW(NULL, IDC_ARROW);

	if (RegisterClassW(&window_class) == 0) {
		win32_error();
	}

	RECT rect = {0, 0, 512, 512};
	DWORD style = WS_CAPTION | WS_SYSMENU | WS_VISIBLE;
	AdjustWindowRect(&rect, style);

	HWND window = CreateWindowW(
			window_class.lpszClassName,
			L"",
			style,
			0, 0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL, NULL,
			instance,
			NULL
	);

	if (window == NULL) {
		win32_error();
	}
	return window;
}

void bee__window_destroy(EGLNativeWindowType window) {
	DestroyWindow(window);
}
