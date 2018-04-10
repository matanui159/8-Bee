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
#include <mint.h>
#include <windows.h>
#include <stdlib.h>

#define MAKEINTATOM_W(atom) (WCHAR*)MAKEINTATOM(atom)

static HWND g_window;

static void message_destroy(void* data) {
	LocalFree((HLOCAL)data);
}

static void win32_error() {
	DWORD error = GetLastError();
	char* message;
	if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error, 0, (char*)&message, 0, NULL) == 0) {
		mint_fail("WIN32: %li (%li)", error, GetLastError());
	} else {
		mint_create(message, message_destroy);
		mint_fail("WIN32: %s", message);
	}
}

static void class_destroy(void* data) {
	UnregisterClassW(MAKEINTATOM_W(*(ATOM*)data), GetModuleHandleW(NULL));
	free(data);
}

static void window_destroy(void* data) {
	DestroyWindow((HWND)data);
}

static LRESULT CALLBACK window_proc(HWND wnd, UINT msg, WPARAM wpm, LPARAM lpm) {
	static _Bool show_cursor = 1;
	switch (msg) {
	case WM_MOUSEMOVE:
		if (show_cursor) {
			ShowCursor(FALSE);
			show_cursor = 0;

			TRACKMOUSEEVENT track = {sizeof(TRACKMOUSEEVENT)};
			track.dwFlags = TME_LEAVE;
			track.hwndTrack = wnd;
			TrackMouseEvent(&track);
		}
		break;
	case WM_MOUSELEAVE:
		ShowCursor(TRUE);
		show_cursor = 1;
		break;
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
	window_class.lpszClassName = L"8bee";
	window_class.lpfnWndProc = window_proc;
	ATOM atom = RegisterClassW(&window_class);
	if (atom == 0) {
		win32_error();
	}

	ATOM* patom = malloc(sizeof(ATOM));
	*patom = atom;
	mint_create(patom, class_destroy);

	static const int size = 512;
	static const DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	RECT rect;
	rect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - size / 2;
	rect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - size / 2;
	rect.right = rect.left + size;
	rect.bottom = rect.top + size;
	AdjustWindowRect(&rect, style, FALSE);

	g_window = CreateWindowW(
			MAKEINTATOM_W(atom),
			L"",
			style,
			rect.left,
			rect.top,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL, NULL,
			instance,
			NULL
	);

	if (g_window == NULL) {
		win32_error();
	}
	mint_create(g_window, window_destroy);
}

void bee__window_update() {
	MSG msg;
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
		DispatchMessageW(&msg);
		if (msg.message == WM_QUIT) {
			ShowWindow(g_window, SW_HIDE);
			exit(msg.wParam);
		}
	}
}

void* bee__window_get() {
	return g_window;
}

void bee__window_show() {
	ShowWindow(g_window, SW_SHOW);
}
