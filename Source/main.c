/*
 * main.c
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

#include <8bee.h>
#include "log.h"
#include "transform.h"
#include "window.h"
#include "video.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

static bee_callback_t g_scene = bee_main;
static void* g_scene_data;

static void signal_error(int signal) {
	const char* message = NULL;
	switch (signal) {
	case SIGILL:
		message = "Illegal operation";
		break;
	case SIGSEGV:
		message = "Segment violation";
		break;
	case SIGFPE:
		message = "Floating point error";
		break;
	}

	if (message == NULL) {
		bee__log_fail("SIG: %i (Unknown signal)", signal);
	} else {
		bee__log_fail("SIG: %s", message);
	}
	exit(signal);
}

void bee_scene(bee_callback_t scene, void* data) {
	g_scene = scene;
	g_scene_data = data;
}

int main(int argc, char* argv[]) {
	_Bool editor = 0;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "editor") == 0) {
			editor = 1;
		} else {
			bee__log_warn("ARG: Unknown command '%s'", argv[i]);
		}
	}

	signal(SIGILL, signal_error);
	signal(SIGSEGV, signal_error);
	signal(SIGFPE, signal_error);

	bee__log_init();
	bee__transform_init();
	bee__window_init();
	bee__video_init();

	if (editor) {
		bee__log_info("ARG: Starting editor");
	} else {
		// bee__res_init();
	}

	bee__window_post_init();

	for (;;) {
		bee__window_update();
		g_scene(g_scene_data);
		bee__video_update();
	}
}
