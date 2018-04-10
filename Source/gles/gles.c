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
#include <mint.h>
#include <stdlib.h>

typedef struct gles_obj_t {
	GLuint data;
	bee__gles_callback_t destroy;
} gles_obj_t;

void bee__gles_init() {
	bee__glext_debug_init();
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

static void gles_destroy(void* data) {
	gles_obj_t* obj = data;
	obj->destroy(obj->data);
	free(obj);
}

GLuint* bee__gles_create(GLuint data, bee__gles_callback_t destroy) {
	gles_obj_t* obj = malloc(sizeof(gles_obj_t));
	obj->data = data;
	obj->destroy = destroy;
	mint_create(obj, gles_destroy);
	return (GLuint*)obj;
}

static void shader_check_error(GLuint object, GLenum STATUS, PFNGLGETSHADERIVPROC Getiv, PFNGLGETSHADERINFOLOGPROC GetInfoLog) {
	GLint length;
	Getiv(object, GL_INFO_LOG_LENGTH, &length);
	if (length > 0) {
		char message[length];
		GetInfoLog(object, length, NULL, message);

		GLint status;
		Getiv(object, STATUS, &status);
		const char* format = "GLSL: %s";
		if (status) {
			mint_warn(format, message);
		} else {
			mint_fail(format, message);
		}
	}
}

static void shader_destroy(GLuint data) {
	glDeleteShader(data);
}

static void program_destroy(GLuint data) {
	glDeleteProgram(data);
}

static GLuint* shader_compile(GLenum type, const char* code) {
	GLuint* shader = bee__gles_create(glCreateShader(type), shader_destroy);
	glShaderSource(*shader, 1, &code, NULL);
	glCompileShader(*shader);
	shader_check_error(*shader, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
	return shader;
}

GLuint bee__gles_shader(const char* vert, const char* frag) {
	GLuint* vertex = shader_compile(GL_VERTEX_SHADER, vert);
	GLuint* fragment = shader_compile(GL_FRAGMENT_SHADER, frag);
	GLuint program = glCreateProgram();
	bee__gles_create(program, program_destroy);

	glAttachShader(program, *vertex);
	glAttachShader(program, *fragment);
	glLinkProgram(program);
	shader_check_error(program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);

	glDetachShader(program, *vertex);
	glDetachShader(program, *fragment);
	mint_destroy(vertex);
	mint_destroy(fragment);
	return program;
}
