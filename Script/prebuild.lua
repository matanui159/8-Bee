local colors = {
		0x0000, 0x005F, 0x00AF, 0x00FF, 0x050F, 0x055F, 0x05AF, 0x05FF,
		0x0A0F, 0x0A5F, 0x0AAF, 0x0AFF, 0x0F0F, 0x0F5F, 0x0FAF, 0x0FFF,
		0x500F, 0x505F, 0x50AF, 0x50FF, 0x550F, 0x555F, 0x55AF, 0x55FF,
		0x5A0F, 0x5A5F, 0x5AAF, 0x5AFF, 0x5F0F, 0x5F5F, 0x5FAF, 0x5FFF,
		0xA00F, 0xA05F, 0xA0AF, 0xA0FF, 0xA50F, 0xA55F, 0xA5AF, 0xA5FF,
		0xAA0F, 0xAA5F, 0xAAAF, 0xAAFF, 0xAF0F, 0xAF5F, 0xAFAF, 0xAFFF,
		0xF00F, 0xF05F, 0xF0AF, 0xF0FF, 0xF50F, 0xF55F, 0xF5AF, 0xF5FF,
		0xFA0F, 0xFA5F, 0xFAAF, 0xFAFF, 0xFF0F, 0xFF5F, 0xFFAF, 0xFFFF
}

local function stream_in(path)
	local file = io.open(path, "rb")
	return function(close)
		if close then
			file:close()
		else
			return file:read(1)
		end
	end
end

local function stream_out(path)
	local file = io.open(path, "wb")
	return function(value, close)
		if close then
			file:close()
		else
			file:write(value)
		end
	end
end

local function stream_dds(stream)
	local function in16(stream)
		local b0 = stream():byte(1)
		local b1 = stream():byte(1)
		return b0 | (b1 << 8)
	end
	local function in32(stream)
		local b0 = in16(stream)
		local b1 = in16(stream)
		return b0 | (b1 << 16)
	end

	in32(stream)
	local header = in32(stream)
	for i = 5, header do
		stream()
	end
	return function(close)
		if close then
			stream(close)
		else
			local pixel = in16(stream)
			return ((pixel << 4) & 0xFFF0) | (pixel >> 12)
		end
	end
end

local function stream_c(var, stream)
	stream("static const " .. var .. "[]={")

	local first = true
	return function(value, close)
		if close then
			stream("};")
			stream(nil, close)
		else
			if not first then
				stream(",")
			end
			stream(value)
			first = false
		end
	end
end

local function stream_bee(stream)
	local color = 0
	local count = 0
	local function flush(stream)
		if count > 0 then
			local large = true
			for i, c in ipairs(colors) do
				if c == color then
					stream(i - 1)
					large = false
					break
				end
			end
			if large then
				stream((color >> 12) | 0x40)
				stream((color >> 4) & 0xFF)
			end
			
			count = count - 1
			if count > 0 then
				while count > 128 do
					stream(0xFF)
					count = count - 128
				end
				stream((count - 1) | 0x80)
				count = 0
			end
		end
	end

	stream(0x22)
	stream(0x01)
	stream(0x04)
	stream(0x80)
	stream(0x15)

	return function(value, close)
		if close then
			flush(stream)
			stream(0x1A)
			stream(nil, close)
		else
			if value & 0x8 then
				value = value | 0xF
			else
				value = 0
			end
			if value == color then
				count = count + 1
			else
				flush(stream)
				color = value
				count = 1
			end
		end
	end
end

local function dds2c(file, var)
	local cfile = file:gsub("dds", "h")
	local input = stream_dds(stream_in(file))
	local image = {}
	for y = 128, 1, -1 do
		for x = 1, 128 do
			image[(y - 1) * 128 + x] = input()
		end
	end
	input(true)

	local output = stream_bee(stream_c("unsigned char " .. var, stream_out(cfile)))
	for i = 1, 128 * 128 do
		output(image[i])
	end
	output(nil, true)
end

local function glsl2c(file, var)
	local cfile = file:gsub("glsl", "h")
	local input = stream_in(file)
	local output = stream_c("char " .. var, stream_out(cfile))
	while true do
		local value = input()
		if value then
			output(value:byte(1))
		else
			output(0)
			break
		end
	end
	output(nil, true)
	input(true)
end

glsl2c("../Source/gles/res/shader_main_vert.glsl", "bee__res_shader_main_vert")
glsl2c("../Source/gles/res/shader_main_frag.glsl", "bee__res_shader_main_frag")
dds2c("../Source/editor/res/editor.dds", "bee__editor_res_editor")