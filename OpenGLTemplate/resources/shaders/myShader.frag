#version 400 core

in vec4 colour;
in vec2 texCoord;
in vec3 normal;
//in vec3 FragPos;

out vec4 fragColour;

uniform sampler2D sampler0;  // The texture sampler

void main(){

	vec4 vTexColour = texture(sampler0, texCoord);
	fragColour = vTexColour*vec4(colour);
}