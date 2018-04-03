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
#include "log.h"
#include <GLES2/gl2.h>
#include <stdlib.h>

#define GLSL(code) #code

const GLuint bee__shader_pos = 0;
const GLuint bee__shader_mat0 = 1;
const GLuint bee__shader_mat1 = 2;
const GLuint bee__shader_sprite = 3;

static GLuint g_program;

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
			bee__log_warn(format, message);
		} else {
			bee__log_fail(format, message);
			exit(EXIT_FAILURE);
		}
	}
}

static GLuint shader_compile(GLenum type, const char* code) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
	shader_check_error(shader, GL_COMPILE_STATUS, glGetShaderiv, glGetShaderInfoLog);
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

	GLuint fragment = shader_compile(GL_FRAGMENT_SHADER, GLSL(
			precision mediump float;
			varying vec2 texcoord;
			uniform sampler2D texture;

			void main() {
				gl_FragColor = texture2D(texture, texcoord);
			}
	));

	g_program = glCreateProgram();
	atexit(shader_exit);
	glAttachShader(g_program, vertex);
	glAttachShader(g_program, fragment);
	glBindAttribLocation(g_program, bee__shader_pos, "pos");
	glBindAttribLocation(g_program, bee__shader_mat0, "mat0");
	glBindAttribLocation(g_program, bee__shader_mat1, "mat1");
	glBindAttribLocation(g_program, bee__shader_sprite, "sprite");
	glLinkProgram(g_program);
	shader_check_error(g_program, GL_LINK_STATUS, glGetProgramiv, glGetProgramInfoLog);

	glDetachShader(g_program, vertex);
	glDetachShader(g_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glUseProgram(g_program);
	glReleaseShaderCompiler();
}
