#version 400 core

in vec4 vCol;
in vec2 texCoord;
in vec3 Normal;
//in vec3 FragPos;

out vec4 fragColour;

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;

void main(){

	vec4 vTexColour = texture(sampler0, texCoord);
	if(true){
		fragColour = vTexColour*vec4(vCol);
		
		}
}