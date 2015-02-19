#version 400
in vec3 out_color;

void main() {
	gl_FragColor = vec4(out_color, 0.3);
}