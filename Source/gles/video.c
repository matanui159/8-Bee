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

#include "../video.h"
#include "gles.h"
#include "context.h"
#include <mint.h>

#include "res/shader_main_vert.h"
#include "res/shader_main_frag.h"

#define BUFFER_SIZE 16

typedef struct elem_t {
	bee__matrix_t matrix;
	struct {
		GLubyte x0, y0, x1, y1;
	} sprite;
} elem_t;

static GLuint g_shader;
static GLint g_shader_pos;
static GLint g_shader_mat0;
static GLint g_shader_mat1;
static GLint g_shader_sprite;

static const GLuint g_quad = 1;
static const GLuint g_framebuffer = 1;

static elem_t g_buffer_data[BUFFER_SIZE];
static int g_buffer_count = 0;

static GLuint g_current_texture = 0;

static void GL_APIENTRY gles_error(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar* message, const void* data) {
	const char* module = "???";
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		module = "GLES";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		module = "GLSL";
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		module = "EGL";
		break;
	}

	static const char* format = "%s: %.*s";
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		mint_fail(format, module, length, message);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		mint_warn(format, module, length, message);
		break;
	default:
		mint_info(format, module, length, message);
		break;
	}
}

static void buffer_destroy(GLuint data) {
	glDeleteBuffers(1, &data);
}

static void framebuffer_destroy(GLuint data) {
	glDeleteFramebuffers(1, &data);
}

static void texture_destroy(GLuint data) {
	glDeleteTextures(1, &data);
}

static void video_flush() {
	if (g_buffer_count > 0) {
		for (int i = 0; i < g_buffer_count; ++i) {
			elem_t* elem = g_buffer_data + i;
			glVertexAttrib3fv(glGetAttribLocation(g_shader, "mat0"), &elem->matrix.m00);
			glVertexAttrib3fv(glGetAttribLocation(g_shader, "mat1"), &elem->matrix.m10);
			glVertexAttrib4f(glGetAttribLocation(g_shader, "sprite"),
					elem->sprite.x0 / 255.0,
					elem->sprite.y0 / 255.0,
					elem->sprite.x1 / 255.0,
					elem->sprite.y1 / 255.0
			);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		g_buffer_count = 0;
	}
}

void bee__video_init_native(void* window) {
	bee__context_init(window);
	bee__gles_init();
	if (GL_debug) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(gles_error, NULL);
	}
	mint_info("GLES: %s", (char*)glGetString(GL_RENDERER));

	static const GLfloat quad_data[] = {
			-0.5, -0.5,
			 0.5, -0.5,
			-0.5,  0.5,
			 0.5,  0.5,
			-0.5,  0.5,
			 0.5, -0.5
	};

	g_shader = bee__gles_shader(bee__res_shader_main_vert, bee__res_shader_main_frag);
	g_shader_pos = glGetAttribLocation(g_shader, "pos");
	g_shader_mat0 = glGetAttribLocation(g_shader, "mat0");
	g_shader_mat1 = glGetAttribLocation(g_shader, "mat1");
	g_shader_sprite = glGetAttribLocation(g_shader, "sprite");

	glBindBuffer(GL_ARRAY_BUFFER, g_quad);
	bee__gles_create(g_quad, buffer_destroy);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_data), quad_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(g_shader_pos);
	glVertexAttribPointer(g_shader_pos, 2, GL_FLOAT, GL_FALSE, 0, 0);

	bee__gles_create(g_framebuffer, framebuffer_destroy);
}

void bee__video_update_native() {
	video_flush();
	bee__context_update();
}

void bee__video_clear() {
	g_buffer_count = 0;
	glClear(GL_COLOR_BUFFER_BIT);
}

void* bee__video_texture_create(int width, int height, unsigned short* data) {
	GLuint name;
	glGenTextures(1, &name);
	glBindTexture(GL_TEXTURE_2D, name);
	GLuint* texture = bee__gles_create(name, texture_destroy);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, g_current_texture);
	return texture;
}

void bee__video_texture_update(void* texture, const bee_sprite_t* sprite, unsigned short* data) {
	GLuint name = *(GLuint*)texture;
	glBindTexture(GL_TEXTURE_2D, name);
	glTexSubImage2D(GL_TEXTURE_2D, 0, sprite->x, sprite->y, sprite->w, sprite->h, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
	glBindTexture(GL_TEXTURE_2D, g_current_texture);
}

void bee__video_texture_target(void* texture) {
	video_flush();
	if (texture == NULL) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	} else {
		GLuint name = *(GLuint*)texture;
		glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, name, 0);
	}
}

void bee__video_texture_draw(void* texture, const bee_sprite_t* sprite, const bee__matrix_t* matrix) {
	GLuint name = *(GLuint*)texture;
	if (name != g_current_texture) {
		video_flush();
		glBindTexture(GL_TEXTURE_2D, name);
		g_current_texture = name;
	}

	elem_t* elem = g_buffer_data + g_buffer_count;
	elem->matrix = *matrix;
	elem->matrix.m00 *= sprite->w;
	elem->matrix.m01 *= sprite->h;
	elem->matrix.m10 *= sprite->w;
	elem->matrix.m11 *= sprite->h;

	elem->sprite.x0 = sprite->x * 2;
	elem->sprite.y0 = sprite->y * 2;
	elem->sprite.x1 = (sprite->x + sprite->w - 1) * 2;
	elem->sprite.y1 = (sprite->y + sprite->h - 1) * 2;
	++g_buffer_count;
	if (g_buffer_count == BUFFER_SIZE) {
		video_flush();
	}
}
