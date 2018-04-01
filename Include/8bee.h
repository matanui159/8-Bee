/*
 * 8bee.h
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

#ifndef _8BEE_H_
#define _8BEE_H_

typedef void (*bee_callback_t)(void* data);

typedef struct bee_sprite_t {
	int x;
	int y;
	int w;
	int h;
	int mask;
} bee_sprite_t;

typedef struct bee_clip_t {
	int* samples;
	int length;
} bee_clip_t;

void bee_scene(bee_callback_t scene, void* data);
unsigned char bee_input();
void bee_draw(bee_sprite_t* sprite);
void bee_play(bee_clip_t* clip, bee_callback_t end);
void bee_savedata(void* data, int length);

void bee_push();
void bee_pop();
void bee_identity();
void bee_translate(int x, int y);
void bee_scale(int w, int h);
void bee_rotate(int angle);

#endif
