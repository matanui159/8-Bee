/*
 * error.c
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

#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void bee__error(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	fflush(stderr);
	va_end(args);

	va_start(args, format);
	int length = vsnprintf(NULL, 0, format, args) + 1;
	va_end(args);

	char message[length];
	va_start(args, format);
	vsnprintf(message, length, format, args);
	va_end(args);
	bee__error_native(message);
}
