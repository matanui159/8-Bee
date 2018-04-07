/*
 * gles.c
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

#include "gles.h"
#include "../log.h"
#include <stdlib.h>

void bee__gles_init() {
	bee__glext_debug_init();
}

void bee__gles_check_error() {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		const char* message = NULL;
		switch (error) {
		case GL_OUT_OF_MEMORY:
			message = "Out of memory";
			break;
		case GL_INVALID_VALUE:
			message = "Invalid value";
			break;
		case GL_INVALID_ENUM:
			message = "Invalid enum";
			break;
		case GL_INVALID_OPERATION:
			message = "Invalid operation";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			message = "Invalid framebuffer operation";
			break;
		}

		if (message == NULL) {
			bee__log_fail("GLSL: %i (Unknown error)", error);
		} else {
			bee__log_fail("GLSL: %s", message);
		}
		exit(error);
	}
}

_Bool bee__gles_check_extension(const char* name) {
	int index = 0;
	for (const char* ext = (char*)glGetString(GL_EXTENSIONS);; ++ext) {
		if (*ext == ' ' || *ext == '\0') {
			if (name[index] == '\0') {
				return 1;
			} else if (*ext == '\0') {
				return 0;
			}
			index = 0;
		} else if (*ext != name[index++]) {
			index = 0;
		}
	}
	return 0;
}

void bee__gles_texture(GLuint texture, const GLushort* data) {
	_Bool update = glIsTexture(texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (update) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 128, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

static _Bool shader_check_error(GLuint object, GLenum STATUS, PFNGLGETSHADERIVPROC Getiv, PFNGLGETSHADERINFOLOGPROC GetInfoLog) {
	GLint status;
	Getiv(object, STATUS, &status);
	GLint length;
	Getiv(object, GL_INFO_LOG_LENGTH, &length);
	if (length > 0) {
		char message[length];
		GetInfoLog(object, length, NULL, message);
		const char* format = "GLSL: %s";
		if (status) {
			bee__log_warn(format, message);
		} else {
			bee__log_fail(format, message);
		}
	}
	return status;
}

static GLuint shader_compile(GLenum type, const char* code) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
	if (!shader_check_error(shader, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog)) {
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint bee__gles_shader(const char* vert, const char* frag) {
	GLuint vertex = shader_compile(GL_VERTEX_SHADER, vert);
	if (vertex == 0) {
		exit(EXIT_FAILURE);
	}

	GLuint fragment = shader_compile(GL_FRAGMENT_SHADER, frag);
	if (fragment == 0) {
		glDeleteShader(vertex);
		exit(EXIT_FAILURE);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glDetachShader(program, vertex);
	glDetachShader(program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	if (!shader_check_error(program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog)) {
		glDeleteProgram(program);
		exit(EXIT_FAILURE);
	}
	return program;
}
