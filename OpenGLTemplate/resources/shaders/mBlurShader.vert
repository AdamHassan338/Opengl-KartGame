#version 400 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTex;

out vec2 vTexcoord;

uniform mat4 model;


void main()
{
    gl_Position = vec4(inPos,0.0, 1.0);
    vTexcoord = inTex;
}
