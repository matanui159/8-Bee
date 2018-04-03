/*
 * debug.h
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

#ifndef GLEXT_DEBUG_H_
#define GLEXT_DEBUG_H_
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

extern _Bool bee__GL_debug;
extern PFNGLDEBUGMESSAGECALLBACKKHRPROC bee__glDebugMessageCallback;
extern PFNGLDEBUGMESSAGEINSERTKHRPROC bee__glDebugMessageInsert;

#define GL_debug bee__GL_debug
#define GL_DEBUG_SOURCE_API GL_DEBUG_SOURCE_API_KHR
#define GL_DEBUG_SOURCE_SHADER_COMPILER GL_DEBUG_SOURCE_SHADER_COMPILER_KHR
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR
#define GL_DEBUG_SOURCE_APPLICATION GL_DEBUG_SOURCE_APPLICATION_KHR
#define GL_DEBUG_TYPE_OTHER GL_DEBUG_TYPE_OTHER_KHR
#define GL_DEBUG_SEVERITY_LOW GL_DEBUG_SEVERITY_LOW_KHR
#define GL_DEBUG_SEVERITY_HIGH GL_DEBUG_SEVERITY_HIGH_KHR
#define glDebugMessageCallback bee__glDebugMessageCallback
#define glDebugMessageInsert bee__glDebugMessageInsert

void bee__glext_debug_init();

#endif
