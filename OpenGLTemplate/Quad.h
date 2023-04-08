#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"
// Class for generating a unit cube
class Quad
{
public:
	Quad();
	~Quad();
	void Create(string filename);
	void Render();
	void RenderNoTexture();
	void Release();
private:
	std::vector<glm::vec2> m_verticies = { glm::vec2(-1,1),glm::vec2(-1,-1) ,glm::vec2(1,1) ,glm::vec2(1,-1) };
	//std::vector<glm::vec2> m_verticies = { glm::vec2(0.0f, 1.0f),glm::vec2(0.0f, 0.0f) ,glm::vec2(1.0f, 1.0f) ,glm::vec2(1.0f, 0.0f) };
	std::vector<glm::vec2> m_tex = { glm::vec2(0,1),glm::vec2(0,0) ,glm::vec2(1,1) ,glm::vec2(1,0) };

	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
};