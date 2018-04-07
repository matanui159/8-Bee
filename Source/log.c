/*
 * log.c
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

#include "log.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

static FILE* g_log;

static void log_exit() {
	fclose(g_log);
}

void bee__log_init() {
	g_log = fopen("8bee.log", "w");
	if (g_log == NULL) {
		bee__log_warn("LOG: %s", strerror(errno));
	} else {
		atexit(log_exit);
	}
}

static char* log_va(const char* prefix, const char* format, va_list list) {
	va_list copy;
	va_copy(copy, list);
	int length = vsnprintf(NULL, 0, format, copy) + 1;
	va_end(copy);
	char* message = malloc(length);
	vsnprintf(message, length, format, list);

	printf("%s%s\n", prefix, message);
	fflush(stdout);
	if (g_log != NULL) {
		fprintf(g_log, "%s%s\n", prefix, message);
	}
	return message;
}

void bee__log_info(const char* format, ...) {
	va_list list;
	va_start(list, format);
	free(log_va("", format, list));
	va_end(list);
}

void bee__log_warn(const char* format, ...) {
	va_list list;
	va_start(list, format);
	free(log_va("!", format, list));
	va_end(list);
}

void bee__log_fail(const char* format, ...) {
	va_list list;
	va_start(list, format);
	const char* message = log_va("!", format, list);
	va_end(list);
	static _Bool fail = 0;
	if (!fail) {
		bee__log_fail_native(message);
		fail = 1;
	}
}
