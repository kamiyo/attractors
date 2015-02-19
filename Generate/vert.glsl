#version 400

uniform mat4 camera;
in vec3 vp;
in vec3 color;
out vec3 out_color;
void main() {
	out_color = color;
	gl_Position = camera * vec4(vp, 1.0);
	//gl_Position = vec4(vp, 1.0);
}