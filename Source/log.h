/*
 * log.h
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

#ifndef LOG_H_
#define LOG_H_

#ifdef __GNUC__
#	define BEE__LOG_FORMAT __attribute__((format(printf, 1, 2)))
#else
#	define BEE__LOG_FORMAT
#endif

void bee__log_init();

void bee__log_info(const char* format, ...) BEE__LOG_FORMAT;
void bee__log_warn(const char* format, ...) BEE__LOG_FORMAT;
void bee__log_fail(const char* format, ...) BEE__LOG_FORMAT;

void bee__log_fail_native(const char* message);

#endif
