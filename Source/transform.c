/*
 * transform.c
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

#include "transform.h"
#include <8bee.h>
#include <stdlib.h>
#include <math.h>

static struct {
	bee__matrix_t* matrices;
	int index;
	int size;
} g_stack;

static void transform_exit() {
	free(g_stack.matrices);
}

void bee__transform_init() {
	g_stack.matrices = malloc(sizeof(bee__matrix_t));
	atexit(transform_exit);
	bee_identity();
	g_stack.size = 1;
}

bee__matrix_t* bee__transform_get() {
	return g_stack.matrices + g_stack.index;
}

void bee_push() {
	if (++g_stack.index == g_stack.size) {
		g_stack.matrices = malloc(sizeof(bee__matrix_t) * (g_stack.size *= 2));
	}
	g_stack.matrices[g_stack.index] = g_stack.matrices[g_stack.index - 1];
}

void bee_pop() {
	--g_stack.index;
}

void bee_identity() {
	bee__matrix_t* matrix = bee__transform_get();
	matrix->m00 = 1 / 64.0;
	matrix->m01 = 0;
	matrix->m02 = 0;
	matrix->m10 = 0;
	matrix->m11 = 1 / 64.0;
	matrix->m12 = 0;
}

void bee_translate(int x, int y) {
	bee__matrix_t* matrix = bee__transform_get();
	matrix->m02 += x;
	matrix->m12 += y;
}

void bee_scale(int w, int h) {
	bee__matrix_t* matrix = bee__transform_get();
	matrix->m00 *= w;
	matrix->m01 *= w;
	matrix->m02 *= w;
	matrix->m10 *= h;
	matrix->m11 *= h;
	matrix->m12 *= h;
}

void bee_angle(int angle) {
	static float deg2rad = 0.01745329;
	float rad = angle * deg2rad;
	float c = cos(rad);
	float s = sin(rad);

	bee__matrix_t* matrix = bee__transform_get();
	matrix->m00 = matrix->m00*c - matrix->m10*s;
	matrix->m01 = matrix->m01*c - matrix->m11*s;
	matrix->m02 = matrix->m02*c - matrix->m12*s;
	matrix->m10 = matrix->m00*s + matrix->m10*c;
	matrix->m11 = matrix->m01*s + matrix->m11*c;
	matrix->m12 = matrix->m02*s + matrix->m12*c;
}
