local colors = {}

local shades = {0x0, 0x5, 0xA, 0xF}
for _, r in ipairs(shades) do
	for _, g in ipairs(shades) do
		for _, b in ipairs(shades) do
			if r ~= 0 or g ~= 0 or b ~= 0 then
				table.insert(colors, (r << 12) | (g << 8) | (b << 4) | 0xF)
			end
		end
	end
end

local function file_newext(file, ext)
	return file:gsub("%.[^/\\]*$", "." .. ext)
end

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

local function stream_c(type, prefix, file, stream)
	local name = file:match("([^/\\%.]*)%.[^/\\]*$")
	stream("static const " .. type .. " " .. prefix .. name .. "[]={")

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
	local prev = 0
	local count = 0
	local function flush(stream)
		if count > 0 then
			if prev >= 64 then
				stream(0x40)
				stream((prev - 64) >> 8)
				stream((prev - 64) & 0xFF)
			else
				stream(prev)
			end
			count = count - 1
			if count > 0 then
				while count > 128 do
					stream(0xFF)
					count = count - 128
				end
				stream((count - 1) | 0x80)
			end
		end
		count = 0
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
			local v = value + 64
			if value & 0xF == 0 then
				v = 0
			else
				for i, c in ipairs(colors) do
					if value == c then
						v = i
						break
					end
				end
			end

			if v == prev then
				count = count + 1
			else
				flush(stream)
				prev = v
				count = 1
			end
		end
	end
end

local function dds2c(file, prefix)
	local cfile = file_newext(file, "h")
	local input = stream_dds(stream_in(file))
	local image = {}
	for y = 128, 1, -1 do
		for x = 1, 128 do
			image[(y - 1) * 128 + x] = input()
		end
	end
	input(true)

	local output = stream_bee(stream_c("unsigned char", prefix, cfile, stream_out(cfile)))
	for i = 1, 128 * 128 do
		output(image[i])
	end
	output(nil, true)
end

local function glsl2c(file, prefix)
	local cfile = file_newext(file, "h")
	local input = stream_in(file)
	local output = stream_c("char", prefix, cfile, stream_out(cfile))
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

glsl2c("../Source/gles/res/shader_main_vert.glsl", "bee__res_")
glsl2c("../Source/gles/res/shader_main_frag.glsl", "bee__res_")
dds2c("../Source/editor/res/editor.dds", "bee__editor_res_")