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
#include <8bee.h>
#include "../log.h"
#include "context.h"
#include "gles.h"
#include "../transform.h"
#include <stdlib.h>
#include <stddef.h>

#include "res/shader_main_vert.h"
#include "res/shader_main_frag.h"

typedef struct elem_t {
	bee__matrix_t matrix;
	struct {
		GLubyte x0, y0, x1, y1;
	} sprite;
} elem_t;

static GLuint g_shader;
static const GLuint g_framebuffer = 1;
static const GLuint g_texdata = 2;
static const GLuint g_quad = 1;

static elem_t g_buffer_data[1];

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
		bee__log_fail(format, module, length, message);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		bee__log_warn(format, module, length, message);
		break;
	case GL_DEBUG_SEVERITY_LOW:
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		bee__log_info(format, module, length, message);
		break;
	}
}

static void video_exit() {
	glDeleteProgram(g_shader);
	glDeleteFramebuffers(1, &g_framebuffer);
	glDeleteTextures(1, &g_framebuffer);
	glDeleteTextures(1, &g_texdata);
	glDeleteBuffers(1, &g_quad);
}

void bee__video_init() {
	bee__context_init();
	bee__gles_init();
	atexit(video_exit);
	if (GL_debug) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(gles_error, NULL);
	}
	bee__log_info("GLES: %s", (char*)glGetString(GL_RENDERER));

	g_shader = bee__gles_shader(bee__res_shader_main_vert, bee__res_shader_main_frag);

	glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
	bee__gles_texture(g_framebuffer, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_framebuffer, 0);
	bee__gles_texture(g_texdata, NULL);

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

	GLint pos = glGetAttribLocation(g_shader, "pos");
	glEnableVertexAttribArray(pos);
	glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
	bee__gles_check_error();
}

void bee__video_data(GLushort* data) {
	bee__gles_texture(g_texdata, NULL);
}

static void video_flush() {
	elem_t* elem = g_buffer_data;
	glVertexAttrib3fv(glGetAttribLocation(g_shader, "mat0"), &elem->matrix.m00);
	glVertexAttrib3fv(glGetAttribLocation(g_shader, "mat0"), &elem->matrix.m10);
	glVertexAttrib4f(glGetAttribLocation(g_shader, "sprite"),
			elem->sprite.x0 / 255.0,
			elem->sprite.y0 / 255.0,
			elem->sprite.x1 / 255.0,
			elem->sprite.y1 / 255.0
	);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
}

void bee__video_pre_update() {
	glClear(GL_COLOR_BUFFER_BIT);
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
	bee__context_update();
}
