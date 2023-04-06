#version 400 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D sampler0;


void main()
{
    vec4 sampled = texture(sampler0, texCoord);
    FragColor = sampled;
    //FragColor = vec4(1.0);
}
