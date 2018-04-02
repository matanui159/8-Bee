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
#include "error.h"
#include <GLES2/gl2.h>
#include <stdlib.h>

static const GLuint g_framebuffer = 1;
static const GLuint g_texdata = 2;
static const GLuint g_quad = 1;
static const GLuint g_buffer = 2;

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
//	glEnableVertexAttribArray();

	atexit(video_exit);
	gles_check_error();
}
