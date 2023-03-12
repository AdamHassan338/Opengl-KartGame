#pragma once

#include "Texture.h"
#include "VertexBufferObjectIndexed.h"

// Class for generating a unit sphere
class MyObject
{
public:
	MyObject();
	~MyObject();
	void Create(string directory, string front, int slicesIn, int stacksIn);
	
	void combine();
	void Render();
	void Release();
private:
	UINT m_vao;
	CVertexBufferObjectIndexed m_vbo;
	CTexture m_texture;
	string m_directory;
	string m_filename;
	int m_numTriangles;

	UINT vao;
	UINT vboInd;
	UINT vboVert;


	std::vector<std::string> list;
	std::vector<glm::vec3> m_vertexData;
	std::vector<float> m_data;
	std::vector<float> m_uv = {
		0.0, 0.0,
		0.5, 1.0,
		1.0, 1.0,
		1.0, 1.0,
		0.0, 1.0,
		1.0, 0.0
	};

	std::vector<glm::vec3> m_vertices = {
		glm::vec3(0.f, 2.f, 0.f),// 0
		glm::vec3(0.f, 0.f, -1.f),//1
		glm::vec3(1.f, 0.f, 0.f),// 2
		glm::vec3(0.f, 0.f, 1.f),// 3
		glm::vec3(-1.f, 0.f, 0.f)//4
	};
	std::vector<GLuint> m_indices = {
	0,2,1,//front
	0,3,2,//right
	0,4,3,//back
	0,1,4,//left
	3,4,1,//bottom left 
	3,1,2//bottom right
	
	};

	std::vector<glm::vec3> m_normals = {
		glm::cross(m_vertices.at(m_indices.at(0)) - m_vertices.at(m_indices.at(1)),m_vertices.at(m_indices.at(0)) - m_vertices.at(m_indices.at(2))),//front
		//glm::cross(m_vertices.at(m_indices.at(0)) - m_vertices.at(m_indices.at(2)),m_vertices.at(m_indices.at(3)) - m_vertices.at(m_indices.at(2))),//right
		glm::cross( glm::vec3(0.f, 0.f, 1.f) -glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 2.f, 0.f) - glm::vec3(1.f, 0.f, 0.f)),// not sure why this returns none zero vector but above comment does
		glm::cross(glm::vec3(0.f, 2.f, 0.f) - glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 2.f, 0.f) - glm::vec3(0.f, 0.f, 1.f)),
		//glm::cross(m_vertices.at(m_indices.at(0)) - m_vertices.at(m_indices.at(4)),m_vertices.at(m_indices.at(0)) - m_vertices.at(m_indices.at(3))),//back
		glm::cross(m_vertices.at(m_indices.at(0)) - m_vertices.at(m_indices.at(4)),m_vertices.at(m_indices.at(0)) - m_vertices.at(m_indices.at(1))),//left
		glm::cross(m_vertices.at(m_indices.at(3)) - m_vertices.at(m_indices.at(1)),m_vertices.at(m_indices.at(3)) - m_vertices.at(m_indices.at(4))),//bottom left
		glm::cross(m_vertices.at(m_indices.at(3)) - m_vertices.at(m_indices.at(2)),m_vertices.at(m_indices.at(3)) - m_vertices.at(m_indices.at(1))) //bottom right
	};
	std::vector<glm::vec3> m_colour;



	
};