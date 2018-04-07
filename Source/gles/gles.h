/*
 * util.h
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

#ifndef GLES_GLES_H_
#define GLES_GLES_H_
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "glext/debug.h"

void bee__gles_init();
void bee__gles_check_error();
_Bool bee__gles_check_extension(const char* name);

void bee__gles_texture(GLuint texture, const GLushort* data);
GLuint bee__gles_shader(const char* vert, const char* frag);

#endif
