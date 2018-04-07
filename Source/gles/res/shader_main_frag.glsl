precision mediump float;
varying vec2 texcoord;
uniform sampler2D texture;

void main() {
	gl_FragColor = texture2D(texture, texcoord);
}