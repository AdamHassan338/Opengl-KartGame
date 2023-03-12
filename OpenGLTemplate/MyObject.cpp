#include "Common.h"
#include <iostream>
#include <stdio.h>

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
	std::cout << std::endl;
	for (int i = 0; i < m_normals.size(); i++) {
		std::cout << "[ " << m_normals.at(i).x << ", " << m_normals.at(i).y << ", " << m_normals.at(i).z << " ]" << std::endl;
		
	}

	for (int i = 0; i < list.size(); i++) {
		std::cout << "" << list.at(i) << " " << list.at(i++) << " " << list.at(i++) << "\n";
	}

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
	int j = 0;
	std::vector<GLuint> finalindices;
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
			list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/4/" + std::to_string(i + 1));
			finalindices.push_back(row + column);
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
			list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/5/" + std::to_string(i + 1));
			finalindices.push_back(row + column);
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
			list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/1/" + std::to_string(i + 1));
			finalindices.push_back(row + column);
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
			list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/3/" + std::to_string(i + 1));
			finalindices.push_back(row + column);
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
			list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/1/" + std::to_string(i + 1));
			finalindices.push_back(row + column);
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
			list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/6/" + std::to_string(i + 1));
			finalindices.push_back(row + column);
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
		list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/2/" + std::to_string(i + 1));
		finalindices.push_back(row + column);
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
		list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/1/" + std::to_string(i + 1));
		finalindices.push_back(row + column);
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
		list.push_back(std::to_string((int)m_indices.at(row + column) + 1) + "/6/" + std::to_string(i + 1));
		finalindices.push_back(row + column);
		//column++;
		





	}
	m_indices = finalindices;
}

// Render the sphere as a set of triangles
void MyObject::Render()
{
	glBindVertexArray(vao);
	m_texture.Bind();
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, NULL);
	//glDrawArrays(GL_TRIANGLES, 0, m_indices.size());
	//printf("rending\n");

}

// Release memory on the GPU 
void MyObject::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &vao);
	m_vbo.Release();
}