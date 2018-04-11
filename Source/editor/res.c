/*
 * res.c
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

#include "res.h"
#include "../res.h"

#include "res/editor.h"

const bee_sprite_t bee__editor_cursor = {8, 0, 8, 8};
const bee_sprite_t bee__editor_selector = {16, 0, 4, 4};
const bee_sprite_t bee__editor_corner = {0, 94, 34, 34};
const bee_sprite_t bee__editor_title_tab = {32, 0, 12, 8};
const bee_sprite_t bee__editor_title_font = {32, 8, 8, 8};
const bee_sprite_t bee__editor_video_tab = {64, 0, 12, 8};
const bee_sprite_t bee__editor_video_colors = {64, 8, 32, 32};
const bee_sprite_t bee__editor_music_tab = {96, 0, 12, 8};
const bee_sprite_t bee__editor_music_notes = {96, 8, 32, 32};
const bee_sprite_t bee__editor_music_font = {96, 40, 8, 8};
const bee_sprite_t bee__editor_music_corner = {110, 118, 18, 10};

void bee__editor_res_init() {
	bee__res_data(sizeof(bee__editor_res_editor), bee__editor_res_editor);
}
