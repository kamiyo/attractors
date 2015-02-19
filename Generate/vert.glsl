#version 400

uniform mat4 camera;

in vec3 vp;
//in vec3 input_color;
out vec3 color;
void main() {
	color = vec3(0.2);
	gl_Position = camera * vec4(vp, 1.0);
	//gl_Position = vec4(vp, 1.0);
}