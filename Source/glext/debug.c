/*
 * debug.c
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

#include "debug.h"
#include "../log.h"
#include "glext.h"
#include <EGL/egl.h>

_Bool bee__GL_debug = 0;
PFNGLDEBUGMESSAGECALLBACKKHRPROC bee__glDebugMessageCallback;
PFNGLDEBUGMESSAGEINSERTKHRPROC bee__glDebugMessageInsert;

void bee__glext_debug_init() {
	if (bee__glext_check_extension("GL_KHR_debug")) {
		bee__GL_debug = 1;
		bee__glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKKHRPROC)eglGetProcAddress("glDebugMessageCallbackKHR");
		bee__glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTKHRPROC)eglGetProcAddress("glDebugMessageInsertKHR");
		bee__log_info("GLES: GL_debug = true");
	} else {
		bee__log_info("GLES: GL_debug = false");
	}
}
