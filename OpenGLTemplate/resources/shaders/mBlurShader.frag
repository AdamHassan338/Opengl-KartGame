#version 400 core

in vec2 vTexcoord;
out vec4 FragColor;

uniform sampler2D sampler0;
uniform bool blur = true;
uniform float weight[9] = float[](0.0625, 0.125, 0.0625,0.125,0.25, 0.125, 0.0625, 0.125, 0.0625);

void main()
{
    if(!blur){
        FragColor = texture(sampler0, vTexcoord);
        return;
    }
    vec2 texeloffset = 1.0 / textureSize(sampler0, 0);
    vec3 sum;
    vec2 kernalOffset = vec2(-1,1);
    for(int i = 0;i<9;i++){
        sum += texture(sampler0, vTexcoord + kernalOffset*texeloffset).rgb * weight[i];
        if(i%2==0){
            kernalOffset.y-=1;
            kernalOffset.x=-1;
        }else{
            kernalOffset.x+=1; 
        }
    }
   

    FragColor = vec4(sum, 1.0);
}
