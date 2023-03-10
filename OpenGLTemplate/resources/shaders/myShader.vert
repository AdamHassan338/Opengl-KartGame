#version 400 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 vcolour;


out vec4 vCol;
out vec2 texCoord;
out vec3 Normal;
//out vec3 FragPos;

uniform mat4 modelView;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
	gl_Position = projection * modelView * vec4(pos, 1.0);
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
    texCoord=tex;
    vec3 eye = normalize(normalMatrix * normal);

    vCol = glm::vec4(vcolour,1.0f);
    texCoord = tex;
    Normal = normal;

   // Normal = normalize(matrices.normalMatrix * normal)
    //FragPos =  (model * vec4(pos, 1.0)).xyz;
}