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

const GLuint bee__shader_pos = 0;
const GLuint bee__shader_mat0 = 1;
const GLuint bee__shader_mat1 = 2;
const GLuint bee__shader_sprite = 3;

static GLuint g_program;

static _Bool shader_check_error(GLuint object, GLenum STATUS, PFNGLGETSHADERIVPROC Getiv, PFNGLGETSHADERINFOLOGPROC GetInfoLog) {
	GLint status;
	Getiv(object, STATUS, &status);
	if (!status) {
		GLint length;
		Getiv(object, GL_INFO_LOG_LENGTH, &length);
		char* message = malloc(length);
		GetInfoLog(object, length, NULL, message);
		bee__error("GLSL: %s", message);
		free(message);
		return 1;
	}
	return 0;
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

static void shader_exit() {
	glDeleteProgram(g_program);
}

void bee__shader_init() {
	GLuint vertex = shader_compile(GL_VERTEX_SHADER, GLSL(
			attribute vec2 pos;
			attribute vec3 mat0;
			attribute vec3 mat1;
			attribute vec4 sprite;
			varying vec2 texcoord;

			void main() {
				if (pos.x < 0.0) {
					texcoord.x = sprite.x;
				} else {
					texcoord.x = sprite.z;
				}
				if (pos.y < 0.0) {
					texcoord.y = sprite.y;
				} else {
					texcoord.y = sprite.w;
				}

				gl_Position = vec4((mat3(
						mat0.x, mat1.x, 0,
						mat0.y, mat1.y, 0,
						mat0.z, mat1.z, 1
				) * vec3(pos, 1)).xy, 0, 1);
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
	glAttachShader(g_program, vertex);
	glAttachShader(g_program, fragment);
	glBindAttribLocation(g_program, bee__shader_pos, "pos");
	glBindAttribLocation(g_program, bee__shader_mat0, "mat0");
	glBindAttribLocation(g_program, bee__shader_mat1, "mat1");
	glBindAttribLocation(g_program, bee__shader_sprite, "sprite");
	glLinkProgram(g_program);

	glDetachShader(g_program, vertex);
	glDetachShader(g_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glUseProgram(g_program);
	glReleaseShaderCompiler();

	atexit(shader_exit);
	if (shader_check_error(g_program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog)) {
		exit(EXIT_FAILURE);
	}
}
