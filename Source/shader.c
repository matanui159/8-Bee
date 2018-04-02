/*
 * shader.c
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

#include "shader.h"
#include "error.h"
#include <GLES2/gl2.h>
#include <stdlib.h>
#include <stdio.h>

#define GLSL(code) #code

static GLuint g_program;

static _Bool shader_check_error(GLuint object, GLenum STATUS, PFNGLGETSHADERIVPROC Getiv, PFNGLGETSHADERINFOLOGPROC GetInfoLog) {
	GLint status;
	Getiv(object, STATUS, &status);
	if (!status) {
		GLint length;
		Getiv(object, GL_INFO_LOG_LENGTH, &length);
		char* message = malloc(length);
		GetInfoLog(object, length, NULL, message);
		bee__error(message);
		free(message);
		return 1;
	}
	return 0;
}

static void shader_exit() {
	glDeleteProgram(g_program);
}

static GLuint shader_compile(GLenum type, const char* code) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
	if (shader_check_error(shader, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog)) {
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

void bee__shader_init() {
	GLuint vertex = shader_compile(GL_VERTEX_SHADER, GLSL(
			attribute vec3 mat0;
			attribute vec3 mat1;
			attribute vec2 pos;
			attribute vec2 f_texcoord;
			varying vec2 texcoord;

			void main() {
				texcoord = f_texcoord;
				vec3 tpos = mat3(
						mat0.x, mat1.x, 0,
						mat0.y, mat1.y, 0,
						mat0.z, mat1.z, 1
				) * vec3(pos, 1);
				gl_Position = vec4(tpos.xy, 0, tpos.z);
			}
	));
	if (vertex == 0) {
		exit(EXIT_FAILURE);
	}

	GLuint fragment = shader_compile(GL_FRAGMENT_SHADER, GLSL(
			precision mediump float;
			varying vec2 texcoord;
			uniform sampler2D texture;

			void main() {
				gl_FragColor = texture2D(texture, texcoord);
			}
	));
	if (fragment == 0) {
		glDeleteShader(vertex);
		exit(EXIT_FAILURE);
	}

	g_program = glCreateProgram();
	atexit(shader_exit);
	glAttachShader(g_program, vertex);
	glAttachShader(g_program, fragment);
	glLinkProgram(g_program);
	glDetachShader(g_program, vertex);
	glDetachShader(g_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glReleaseShaderCompiler();
	if (shader_check_error(g_program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog)) {
		exit(EXIT_FAILURE);
	}
}
