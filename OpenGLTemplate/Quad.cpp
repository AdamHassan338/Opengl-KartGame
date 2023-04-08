#include "Quad.h"



Quad::Quad()
{
}

Quad::~Quad()
{
}

void Quad::Create(string filename)
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
	for (int i = 0; i < m_verticies.size(); i++) {
		m_vbo.AddData(&m_verticies[i], sizeof(glm::vec2));
		m_vbo.AddData(&m_tex[i], sizeof(glm::vec2));
	}


	// Upload data to GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec2));

}

void Quad::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Quad::RenderNoTexture()
{
	glBindVertexArray(m_vao);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Quad::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}
