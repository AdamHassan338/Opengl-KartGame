#version 400 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTex;
layout (location = 3) in vec3 incolour;


out vec4 colour;
out vec2 texCoord;
out vec3 normal;

out vec3 reflected;

uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform mat4 inverseViewMatrix;

void main()
{
	gl_Position = projection * modelView * vec4(inPos, 1.0);

    vec3 n = normalize(normalMatrix * inNormal);
    vec3 p = (modelView * vec4(inPos, 1.0f)).xyz;
    reflected = (inverseViewMatrix * vec4(reflect(p, n), 1)).xyz;

    texCoord=inTex;
    colour = vec4(incolour,1.0f);
    normal = inNormal;
}