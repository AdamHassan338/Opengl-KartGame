#pragma once

#include "Texture.h"
#include "VertexBufferObject.h"
#include "Cubemap.h"

// This is a class for creating and rendering a skybox
class CSkybox
{
public:
	CSkybox();
	~CSkybox();
	void Create(float size);
	void Render(int textureUnit);
	void Release();
	CCubemap m_cubemapTexture;
private:
	UINT m_vao;
	CVertexBufferObject m_vbo;
	
	
};