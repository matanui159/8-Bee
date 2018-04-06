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

local stream_end = "\0END"

local function stream_in(path)
	local file = io.open(path, "rb")
	return function(value)
		if value == stream_end then
			file:close()
		else
			return file:read(1)
		end
	end
end

local function stream_out(path)
	local file = io.open(path, "wb")
	return function(value)
		if value == stream_end then
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
	return function(value)
		if value == stream_end then
			stream(value)
		else
			local pixel = in16(stream)
			return ((pixel << 4) & 0xFFF0) | (pixel >> 12)
		end
	end
end

local function stream_c(stream, name)
	stream("static const unsigned char " .. name .. "[]={")
	local first = true
	return function(value)
		if value == stream_end then
			stream("};")
			stream(stream_end)
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
			stream(prev)
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

	return function(value)
		if value == stream_end then
			flush(stream)
			stream(0x1A)
			stream(stream_end)
		else
			local v = -1
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

			if v == -1 then
				flush(stream)
				stream(0x40)
				stream(value >> 8)
				stream(value & 0xFF)
			elseif v == prev then
				count = count + 1
			else
				flush(stream)
				prev = v
				count = 1
			end
		end
	end
end

local function dds2c(dds, c, name)
	local input = stream_dds(stream_in(dds))
	local output = stream_bee(stream_c(stream_out(c), name))
	for i = 1, 128 * 128 do
		output(input())
	end
	input(stream_end)
	output(stream_end)
end

return dds2c