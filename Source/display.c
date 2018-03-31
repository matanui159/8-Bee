/*
 * display.c
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

#include "display.h"
#include "window.h"
#include <EGL/egl.h>
#include <stdlib.h>
#include <stdio.h>

static EGLNativeWindowType g_window = NULL;

static void egl_error() {
	static EGLint first_error = 0x3001;
	static EGLint last_error = 0x300E;
	static const char* error_messages[] = {
			"Not initialized",
			"Bad access",
			"Bad alloc",
			"Bad attribute",
			"Bad config",
			"Bad context",
			"Bad current surface",
			"Bad display",
			"Bad match",
			"Bad native pixmap",
			"Bad native window",
			"Bad parameter",
			"Bad surface",
			"Context lost"
	};

	EGLint error = eglGetError();
	if (error > last_error) {
		fprintf(stderr, "%s\n", error_messages[error - first_error]);
	} else {
		fprintf("%i (Unknown error)\n", error);
	}
	exit(EXIT_FAILURE);
}

static void display_exit() {
	EGLDisplay display;
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(display);
	if (g_window != NULL) {
		bee__window_destroy(g_window);
	}
}

void bee__display_init() {
	// display
	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (!eglInitialize(display)) {
		egl_error();
	}
	atexit(display_exit);

	// config
	static const EGLint config_attribs[] = {
			EGL_RED_SIZE, 2,
			EGL_GREEN_SIZE, 2,
			EGL_BLUE_SIZE, 2,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_NONE
	};

	EGLConfig config;
	EGLint count;
	if (!eglChooseConfig(display, config_attribs, &config, 1, &count)) {
		egl_error();
	}
	if (count < 1) {
		fprintf(stderr, "No supported config\n");
	}

	// surface
	EGLNativeWindowType window = bee__window_create();
	EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);

	// context
	static const EGLint context_attribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_NONE
	};

	EGLContext context = eglCreateContext(display, config, NULL, context_attribs);
	eglMakeCurrent(display, surface, surface, context);
}
