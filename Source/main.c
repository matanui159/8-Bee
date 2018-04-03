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
#include "error.h"
#include "transform.h"
#include "window.h"
#include "display.h"
#include "shader.h"
#include "video.h"
#include <signal.h>

static bee_callback_t g_scene = bee_main;
static void* g_scene_data;

static void signal_error(int signal) {
	switch (signal) {
	case SIGILL:
		bee__error("SIG: Illegal operation");
		break;
	case SIGSEGV:
		bee__error("SIG: Segment fault");
		break;
	case SIGFPE:
		bee__error("SIG: Floating point error");
		break;
	}
}

void bee_scene(bee_callback_t scene, void* data) {
	g_scene = scene;
	g_scene_data = data;
}

int main(int argc, char* argv[]) {
	signal(SIGILL, signal_error);
	signal(SIGSEGV, signal_error);
	signal(SIGFPE, signal_error);

	bee__transform_init();
	bee__window_init();
	bee__display_init();
	bee__shader_init();
	bee__video_init();

	for (;;) {
		bee__window_update();
		g_scene(g_scene_data);
		bee__video_update();
		bee__display_update();
	}
}
