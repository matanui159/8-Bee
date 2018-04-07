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