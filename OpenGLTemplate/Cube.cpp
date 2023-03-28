#include "Cube.h"
CCube::CCube()
{}
CCube::~CCube()
{
	Release();
}
void CCube::Create(string filename)
{
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();
	// Write the code to add interleaved vertex attributes to the VBO
	
	glm::vec3 v0 = glm::vec3(-1, -1, 1);
	glm::vec3 v1 = glm::vec3(1, -1, 1);
	glm::vec3 v2 = glm::vec3(1, 1, 1);
	glm::vec3 v3 = glm::vec3(-1, 1, 1);
	glm::vec3 n = glm::vec3(0, 0, 1);
	glm::vec2 t0 = glm::vec2(0, 0);
	glm::vec2 t1 = glm::vec2(1, 0);
	glm::vec2 t2 = glm::vec2(1, 1);
	glm::vec2 t3 = glm::vec2(0, 1);

	m_vbo.AddData(&v0, sizeof(glm::vec3));
	m_vbo.AddData(&t0, sizeof(glm::vec2));
	m_vbo.AddData(&n, sizeof(glm::vec3));
	m_vbo.AddData(&v1, sizeof(glm::vec3));
	m_vbo.AddData(&t1, sizeof(glm::vec2));
	m_vbo.AddData(&n, sizeof(glm::vec3));
	m_vbo.AddData(&v3, sizeof(glm::vec3));
	m_vbo.AddData(&t3, sizeof(glm::vec2));
	m_vbo.AddData(&n, sizeof(glm::vec3));
	m_vbo.AddData(&v2, sizeof(glm::vec3));
	m_vbo.AddData(&t2, sizeof(glm::vec2));
	m_vbo.AddData(&n, sizeof(glm::vec3));


	// Upload data to GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}
void CCube::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	// Call glDrawArrays to render each side
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void CCube::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}