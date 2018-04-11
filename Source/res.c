/*
 * res.c
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

#include "res.h"
#include "video.h"
#include <mint.h>
#include <stdint.h>

typedef struct stream_t {
	int length;
	const unsigned char* data;
	int index;
} stream_t;

static uint8_t res_read8(stream_t* stream) {
	if (stream->index == stream->length) {
		mint_fail("RES: Unexpected end of file");
	}
	return stream->data[stream->index++];
}

static uint16_t res_read16(stream_t* stream) {
	uint8_t b0 = res_read8(stream);
	uint8_t b1 = res_read8(stream);
	return (b0 << 8) | b1;
}

static uint32_t res_read32(stream_t* stream) {
	uint16_t b0 = res_read16(stream);
	uint16_t b1 = res_read16(stream);
	return (b0 << 16) | b1;
}

void bee__res_data(int length, const unsigned char* data) {
	static const uint16_t colors[] = {
			0x0000, 0x005F, 0x00AF, 0x00FF, 0x050F, 0x055F, 0x05AF, 0x05FF,
			0x0A0F, 0x0A5F, 0x0AAF, 0x0AFF, 0x0F0F, 0x0F5F, 0x0FAF, 0x0FFF,
			0x500F, 0x505F, 0x50AF, 0x50FF, 0x550F, 0x555F, 0x55AF, 0x55FF,
			0x5A0F, 0x5A5F, 0x5AAF, 0x5AFF, 0x5F0F, 0x5F5F, 0x5FAF, 0x5FFF,
			0xA00F, 0xA05F, 0xA0AF, 0xA0FF, 0xA50F, 0xA55F, 0xA5AF, 0xA5FF,
			0xAA0F, 0xAA5F, 0xAAAF, 0xAAFF, 0xAF0F, 0xAF5F, 0xAFAF, 0xAFFF,
			0xF00F, 0xF05F, 0xF0AF, 0xF0FF, 0xF50F, 0xF55F, 0xF5AF, 0xF5FF,
			0xFA0F, 0xFA5F, 0xFAAF, 0xFAFF, 0xFF0F, 0xFF5F, 0xFFAF, 0xFFFF
	};

	stream_t stream = {length, data, 0};
	if (res_read32(&stream) != 0x22010480) {
		mint_fail("RES: Invalid header");
	}

	for (;;) {
		uint8_t type = res_read8(&stream);
		if (type == 0x1A) {
			break;
		}

		uint16_t buffer[128 * 128];
		int count = 0;
		for (int i = 0; i < 128 * 128;) {
			if (count > 0) {
				buffer[i] = buffer[i - 1];
				--count;
			} else {
				uint8_t value = res_read8(&stream);
				if (value & 0x80) {
					count = value & 0x7F;
					if (i == 0) {
						mint_fail("RES: Invalid data chunk");
					}
					buffer[i] = buffer[i - 1];
				} else if (value & 0x40) {
					buffer[i] = ((value & 0x0F) << 12) | (res_read8(&stream) << 4) | 0xF;
				} else {
					buffer[i] = colors[value];
				}
			}
		}

		switch (type) {
		case 0x15:
			bee__video_data(buffer);
		}
	}
}
