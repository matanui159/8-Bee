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
#include <8bee.h>
#include "error.h"
#include "transform.h"
#include "shader.h"
#include <GLES2/gl2.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct elem_t {
	bee__matrix_t matrix;
	struct {
		GLubyte x0, y0, x1, y1;
	} sprite;
} elem_t;

static const GLuint g_framebuffer = 1;
static const GLuint g_texdata = 2;
static const GLuint g_quad = 1;
static const GLuint g_buffer = 2;

static elem_t g_buffer_data[1];

static void gles_check_error() {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		switch (error) {
		case GL_OUT_OF_MEMORY:
			bee__error("GLES: Out of memory");
			break;
		case GL_INVALID_VALUE:
			bee__error("GLES: Invalid value");
			break;
		case GL_INVALID_ENUM:
			bee__error("GLES: Invalid enum");
			break;
		case GL_INVALID_OPERATION:
			bee__error("GLES: Invalid operation");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			bee__error("GLES: Invalid framebuffer operation");
			break;
		default:
			bee__error("GLES: %i (Unknown error)", error);
			break;
		}
		exit(EXIT_FAILURE);
	}
}

static void texture_create(GLuint name) {
	glBindTexture(GL_TEXTURE_2D, g_framebuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

static void video_exit() {
	glDeleteFramebuffers(1, &g_framebuffer);
	glDeleteTextures(1, &g_framebuffer);
	glDeleteTextures(1, &g_texdata);
	glDeleteBuffers(1, &g_quad);
	glDeleteBuffers(1, &g_buffer);
}

void bee__video_init() {
	glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
	glBindTexture(GL_TEXTURE_2D, g_framebuffer);
	texture_create(g_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_framebuffer, 0);
	texture_create(g_texdata);

	static const GLfloat quad_data[] = {
			-0.5, -0.5,
			 0.5, -0.5,
			-0.5,  0.5,
			 0.5,  0.5,
			-0.5,  0.5,
			 0.5, -0.5
	};

	glBindBuffer(GL_ARRAY_BUFFER, g_quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(bee__shader_pos);
	glVertexAttribPointer(bee__shader_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, g_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_buffer_data), NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(bee__shader_mat0);
	glEnableVertexAttribArray(bee__shader_mat1);
	glEnableVertexAttribArray(bee__shader_sprite);
	glVertexAttribPointer(bee__shader_mat0, 3, GL_FLOAT, GL_FALSE, sizeof(elem_t), (void*)offsetof(elem_t, matrix.m00));
	glVertexAttribPointer(bee__shader_mat1, 3, GL_FLOAT, GL_FALSE, sizeof(elem_t), (void*)offsetof(elem_t, matrix.m10));
	glVertexAttribPointer(bee__shader_sprite, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(elem_t), (void*)offsetof(elem_t, sprite));

	atexit(video_exit);
	gles_check_error();
}

void bee__video_data(unsigned char* data) {
	GLushort native[128 * 128];
	for (int i = 0; i < 128 * 128; ++i) {
		native[i] = 0x000F;
		native[i] |= (data[i] & 0x30) * 0x0500;
		native[i] |= (data[i] & 0x0C) * 0x0140;
		native[i] |= (data[i] & 0x03) * 0x0050;
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 128, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, native);
	gles_check_error();
}

static void video_flush() {
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_buffer_data), g_buffer_data, GL_STREAM_DRAW);
	gles_check_error();
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_buffer_data), g_buffer_data);
	gles_check_error();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	gles_check_error();
}

void bee_draw(const bee_sprite_t* sprite) {
	elem_t* elem = g_buffer_data + 0;
	elem->matrix = *bee__transform_get();
	elem->matrix.m00 *= sprite->w;
	elem->matrix.m01 *= sprite->h;
	elem->matrix.m10 *= sprite->w;
	elem->matrix.m11 *= sprite->h;

	elem->sprite.x0 = sprite->x * 2;
	elem->sprite.y0 = sprite->y * 2;
	elem->sprite.x1 = (sprite->x + sprite->w - 1) * 2;
	elem->sprite.y1 = (sprite->y + sprite->h - 1) * 2;
	video_flush();
	gles_check_error();
}

void bee__video_update() {
//	video_flush();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, g_framebuffer);

	static const bee_sprite_t sprite = {0, 0, 128, 128};
	bee_push();
	bee_identity();
	bee_draw(&sprite);
//	video_flush();
	bee_pop();

	glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
	glBindTexture(GL_TEXTURE_2D, g_texdata);
	gles_check_error();
}
