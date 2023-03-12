#version 400 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTex;
layout (location = 3) in vec3 incolour;


out vec4 colour;
out vec2 texCoord;
out vec3 normal;

uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
	gl_Position = projection * modelView * vec4(inPos, 1.0);
    texCoord=inTex;
    colour = vec4(incolour,1.0f);
    normal = inNormal;
}