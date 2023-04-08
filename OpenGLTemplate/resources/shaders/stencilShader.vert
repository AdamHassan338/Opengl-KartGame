#version 440

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 inNormal;

uniform mat4 modelView;
uniform mat4 projection;



void main(){
	gl_Position = projection * modelView  * vec4(pos, 1.0);
}
