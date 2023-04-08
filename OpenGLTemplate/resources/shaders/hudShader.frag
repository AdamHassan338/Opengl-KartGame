#version 400 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D sampler0;
uniform bool useTex;
uniform vec3 colour;


void main()
{
    if(useTex){
        vec4 sampled = texture(sampler0, texCoord);
        FragColor = sampled;
        }
   else{
        FragColor = vec4(colour,1.0);         
        }
    //FragColor = vec4(1.0);
}
