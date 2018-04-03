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
#include "error.h"
#include "window.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdlib.h>

static EGLDisplay g_display;
static EGLSurface g_surface;

static void egl_error() {
	static const EGLint first_error = 0x3001;
	static const EGLint last_error = 0x300E;
	static const char* const error_messages[] = {
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
	if (error >= first_error && error <= last_error) {
		bee__error("EGL: %s", error_messages[error - first_error]);
	} else {
		bee__error("EGL: %i (Unknown error)", error);
	}
	exit(error);
}

static void display_exit() {
	eglReleaseThread();
	eglTerminate(g_display);
}

void bee__display_init() {
	// display
	g_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (!eglInitialize(g_display, NULL, NULL)) {
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
	if (!eglChooseConfig(g_display, config_attribs, &config, 1, &count)) {
		egl_error();
	}
	if (count < 1) {
		 bee__error("EGL: No supported config");
		 exit(EXIT_FAILURE);
	}

	// surface
	g_surface = eglCreateWindowSurface(g_display, config, bee__window_get(), NULL);
	if (g_surface == EGL_NO_SURFACE) {
		egl_error();
	}

	// context
	static const EGLint context_debug_attribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 2,
			EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR,
			EGL_NONE
	};

	EGLContext context = eglCreateContext(g_display, config, NULL, context_debug_attribs);
	if (context == EGL_NO_CONTEXT) {
		static const EGLint context_attribs[] = {
				EGL_CONTEXT_CLIENT_VERSION, 2,
				EGL_NONE
		};

		context = eglCreateContext(g_display, config, NULL, context_attribs);
		if (context == EGL_NO_CONTEXT) {
			egl_error();
		}
	}

	eglMakeCurrent(g_display, g_surface, g_surface, context);
}

void bee__display_update() {
	eglSwapBuffers(g_display, g_surface);
}
