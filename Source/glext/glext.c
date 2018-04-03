/*
 * glext.c
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

#include "glext.h"
#include <GLES2/gl2.h>
#include <string.h>

#include "debug.h"

void bee__glext_init() {
	bee__glext_debug_init();
}

_Bool bee__glext_check_extension(const char* name) {
	int index = 0;
	for (const char* ext = (char*)glGetString(GL_EXTENSIONS);; ++ext) {
		if (*ext == ' ' || *ext == '\0') {
			if (name[index] == '\0') {
				return 1;
			} else if (*ext == '\0') {
				return 0;
			}
			index = 0;
		} else if (*ext != name[index++]) {
			index = 0;
		}
	}
	return 0;
}
