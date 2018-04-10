/*
 * video.h
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

#ifndef VIDEO_H_
#define VIDEO_H_
#include <8bee.h>
#include "transform.h"

void bee__video_init_native(void* window);
void bee__video_update_native();
void bee__video_clear();

void* bee__video_texture_create(int width, int height, unsigned short* data);
void bee__video_texture_update(void* texture, const bee_sprite_t* sprite, unsigned short* data);
void bee__video_texture_target(void* texture);
void bee__video_texture_draw(void* texture, const bee_sprite_t* sprite, const bee__matrix_t* matrix);

void bee__video_init();
void bee__video_data(unsigned short* data);
void bee__video_update();

#endif
