#include "Common.h"
#include <iostream>

#define _USE_MATH_DEFINES
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include "MyObject.h"
#include <math.h>

MyObject::MyObject()
{}

MyObject::~MyObject()
{}

// Create a unit sphere 
void MyObject::Create(string a_sDirectory, string a_sFilename, int slicesIn, int stacksIn)
{
	// check if filename passed in -- if so, load texture

	m_texture.Load(a_sDirectory+a_sFilename);

	m_directory = a_sDirectory;
	m_filename = a_sFilename;

	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	combine();



	/*
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	m_vbo.Create();
	m_vbo.Bind();
	
	
	// Compute vertex attributes and store in VBO
	int vertexCount = 0;
	for (int stacks = 0; stacks < stacksIn; stacks++) {
		float phi = (stacks / (float) (stacksIn - 1)) * (float) M_PI;
		for (int slices = 0; slices <= slicesIn; slices++) {
			float theta = (slices / (float) slicesIn) * 2 * (float) M_PI;
			
			glm::vec3 v = glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi));
			glm::vec2 t = glm::vec2(slices / (float) slicesIn, stacks / (float) stacksIn);
			glm::vec3 n = v;

			m_vbo.AddVertexData(&v, sizeof(glm::vec3));
			m_vbo.AddVertexData(&t, sizeof(glm::vec2));
			m_vbo.AddVertexData(&n, sizeof(glm::vec3));

			vertexCount++;

		}
	}

	// Compute indices and store in VBO
	m_numTriangles = 0;
	for (int stacks = 0; stacks < stacksIn; stacks++) {
		for (int slices = 0; slices < slicesIn; slices++) {
			unsigned int nextSlice = slices + 1;
			unsigned int nextStack = (stacks + 1) % stacksIn;

			unsigned int index0 = stacks * (slicesIn+1) + slices;
			unsigned int index1 = nextStack * (slicesIn+1) + slices;
			unsigned int index2 = stacks * (slicesIn+1) + nextSlice;
			unsigned int index3 = nextStack * (slicesIn+1) + nextSlice;

			m_vbo.AddIndexData(&index0, sizeof(unsigned int));
			m_vbo.AddIndexData(&index1, sizeof(unsigned int));
			m_vbo.AddIndexData(&index2, sizeof(unsigned int));
			m_numTriangles++;

			m_vbo.AddIndexData(&index2, sizeof(unsigned int));
			m_vbo.AddIndexData(&index1, sizeof(unsigned int));
			m_vbo.AddIndexData(&index3, sizeof(unsigned int));
			m_numTriangles++;

		}
	}

	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei stride = 2*sizeof(glm::vec3)+sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));*/

	//define VAO ID:
	

	// generate VAO and bind it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//define indices VBO ID:
	

	//generate the indices VBO
	glGenBuffers(1, &vboInd);

	//Bind the indices VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboInd);

	//copy indices to GPU
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices[0]) * m_indices.size(), &m_indices[0], GL_STATIC_DRAW);

	
	//define vertex VBO ID:
	

	//generate the vertex VBO
	glGenBuffers(1, &vboVert);

	//Bind the vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboVert);

	//copy vertex data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_data[0]) * m_data.size(), &m_data[0], GL_STATIC_DRAW);


	//vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(m_data[0]), 0);
	
	//vertex normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(m_data[0]), (const GLvoid*)(3 * sizeof(float)));
	
	//vertex UV attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(m_data[0]), (const GLvoid*)(6 * sizeof(float)));

	//vertex colour attribute
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(m_data[0]), (const GLvoid*)(8 * sizeof(float)));

		
	
}

void MyObject::combine()
{
	std::cout << "comnging";
	int j = 0;
	for (int i = 0; i < 5; i++) {
		int row = i * 3;
		int column = 0;
		if (i == 4 ) {// if bottom faces
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
			m_data.push_back(m_normals.at(i).x);
			m_data.push_back(m_normals.at(i).y);
			m_data.push_back(m_normals.at(i).z);
			m_data.push_back(1.0f);//u
			m_data.push_back(1.0f);//v
			m_data.push_back(1.0f);//R
			m_data.push_back(1.0f);//G
			m_data.push_back(1.0f);//B
			column++;

			m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
			m_data.push_back(m_normals.at(i).x);
			m_data.push_back(m_normals.at(i).y);
			m_data.push_back(m_normals.at(i).z);
			m_data.push_back(0.0f);//u
			m_data.push_back(0.0f);//v
			m_data.push_back(1.0f);//R
			m_data.push_back(1.0f);//G
			m_data.push_back(1.0f);//B

			column++;


			m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
			m_data.push_back(m_normals.at(i).x);
			m_data.push_back(m_normals.at(i).y);
			m_data.push_back(m_normals.at(i).z);
			m_data.push_back(0.0f);//u
			m_data.push_back(1.0f);//v
			m_data.push_back(1.0f);//R
			m_data.push_back(1.0f);//G
			m_data.push_back(1.0f);//B

			column++;

			m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
			m_data.push_back(m_normals.at(i).x);
			m_data.push_back(m_normals.at(i).y);
			m_data.push_back(m_normals.at(i).z);
			m_data.push_back(1.0f);//u
			m_data.push_back(1.0f);//v
			m_data.push_back(1.0f);//R
			m_data.push_back(1.0f);//G
			m_data.push_back(1.0f);//B
			column++;

			m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
			m_data.push_back(m_normals.at(i).x);
			m_data.push_back(m_normals.at(i).y);
			m_data.push_back(m_normals.at(i).z);
			m_data.push_back(1.0f);//u
			m_data.push_back(0.0f);//v
			m_data.push_back(1.0f);//R
			m_data.push_back(1.0f);//G
			m_data.push_back(1.0f);//B

			column++;

			m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
			m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
			m_data.push_back(m_normals.at(i).x);
			m_data.push_back(m_normals.at(i).y);
			m_data.push_back(m_normals.at(i).z);
			m_data.push_back(0.0f);//u
			m_data.push_back(0.0f);//v
			m_data.push_back(1.0f);//R
			m_data.push_back(1.0f);//G
			m_data.push_back(1.0f);//B
			continue;
		}

		m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
		m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
		m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
		m_data.push_back(m_normals.at(i).x);
		m_data.push_back(m_normals.at(i).y);
		m_data.push_back(m_normals.at(i).z);
		m_data.push_back(0.5f);//u
		m_data.push_back(1.0f);//v
		m_data.push_back(1.0f);//R
		m_data.push_back(1.0f);//G
		m_data.push_back(1.0f);//B

		column++;
		m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
		m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
		m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
		m_data.push_back(m_normals.at(i).x);
		m_data.push_back(m_normals.at(i).y);
		m_data.push_back(m_normals.at(i).z);
		m_data.push_back(0.0f);//u
		m_data.push_back(0.0f);//v
		m_data.push_back(1.0f);//R
		m_data.push_back(1.0f);//G
		m_data.push_back(1.0f);//B
		column++;

		m_data.push_back(m_vertices.at(m_indices.at(row + column)).x);
		m_data.push_back(m_vertices.at(m_indices.at(row + column)).y);
		m_data.push_back(m_vertices.at(m_indices.at(row + column)).z);
		m_data.push_back(m_normals.at(i).x);
		m_data.push_back(m_normals.at(i).y);
		m_data.push_back(m_normals.at(i).z);
		m_data.push_back(1.0f);//u
		m_data.push_back(0.0f);//v
		m_data.push_back(1.0f);//R
		m_data.push_back(1.0f);//G
		m_data.push_back(1.0f);//B
		//column++;





	}/*
	
	m_vertexData = {
		m_vertices.at(m_indices.at(0)),			m_normals.at(0),
		m_vertices.at(m_indices.at(1)),			m_normals.at(0),
		m_vertices.at(m_indices.at(2)),			m_normals.at(0),

		m_vertices.at(m_indices.at(3)),			m_normals.at(1),
		m_vertices.at(m_indices.at(4)),			m_normals.at(1),
		m_vertices.at(m_indices.at(5)),			m_normals.at(1),

		m_vertices.at(m_indices.at(6)),			m_normals.at(2),
		m_vertices.at(m_indices.at(7)),			m_normals.at(2),
		m_vertices.at(m_indices.at(8)),			m_normals.at(2),


		m_vertices.at(m_indices.at(9)),			m_normals.at(3),
		m_vertices.at(m_indices.at(10)),		m_normals.at(3),
		m_vertices.at(m_indices.at(11)),		m_normals.at(3),


		m_vertices.at(m_indices.at(12)),		m_normals.at(4),
		m_vertices.at(m_indices.at(13)),		m_normals.at(4),
		m_vertices.at(m_indices.at(14)),		m_normals.at(4),

		m_vertices.at(m_indices.at(15)),		m_normals.at(5),
		m_vertices.at(m_indices.at(16)),		m_normals.at(5),
		m_vertices.at(m_indices.at(17)),		m_normals.at(5),
	};*/

}

// Render the sphere as a set of triangles
void MyObject::Render()
{
	glBindVertexArray(vao);
	m_texture.Bind();
	//glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, m_indices.size());
	printf("rending\n");

}

// Release memory on the GPU 
void MyObject::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &vao);
	m_vbo.Release();
}