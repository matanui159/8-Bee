/*
 * video.c
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

#include "video.h"
#include "window.h"
#include <stddef.h>

static const bee_sprite_t g_all = {0, 0, 128, 128};
static void* g_buffer;
static void* g_texdata;

void bee__video_init() {
	bee__video_init_native(bee__window_get());
	g_buffer = bee__video_texture_create(128, 128, NULL);
	g_texdata = bee__video_texture_create(128, 128, NULL);
	bee__video_texture_target(g_buffer);
}

void bee__video_data(unsigned short* data) {
	bee__video_texture_update(g_texdata, &g_all, data);
}

void bee__video_update() {
	static const bee__matrix_t identity = {
			1 / 64.0, 0,        0,
			0,        1 / 64.0, 0
	};

	bee__video_texture_target(NULL);
	bee__video_texture_draw(g_buffer, &g_all, &identity);
	bee__video_texture_target(g_buffer);
	bee__video_clear();
	bee__video_update_native();
}

void bee_draw(const bee_sprite_t* sprite) {
	bee__video_texture_draw(g_texdata, sprite, bee__transform_get());
}
