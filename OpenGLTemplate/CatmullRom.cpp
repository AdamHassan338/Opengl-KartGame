#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{

}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
	/*
	m_controlPoints.push_back(glm::vec3(100, 10, 0));
	m_controlPoints.push_back(glm::vec3(71, 15, 71));
	m_controlPoints.push_back(glm::vec3(0, 25, 100));
	m_controlPoints.push_back(glm::vec3(-71, 30, 71));
	m_controlPoints.push_back(glm::vec3(-100,30 , 0));
	m_controlPoints.push_back(glm::vec3(-71, 25, -71));
	m_controlPoints.push_back(glm::vec3(0, 15, -100));
	m_controlPoints.push_back(glm::vec3(71, 10, -71));
	*/


	m_controlPoints.push_back(glm::vec3(0, 5, 0));
	m_controlPoints.push_back(glm::vec3(120, 5, 0));
	m_controlPoints.push_back(glm::vec3(270, 25, 125));
	m_controlPoints.push_back(glm::vec3(375, 30, 260));
	m_controlPoints.push_back(glm::vec3(755, 40, 260));
	m_controlPoints.push_back(glm::vec3(870, 20, 460));
	m_controlPoints.push_back(glm::vec3(870, 5, 615));
	m_controlPoints.push_back(glm::vec3(730, 10, 740));
	m_controlPoints.push_back(glm::vec3(310, 0, 740));
	m_controlPoints.push_back(glm::vec3(-76, -10, 740));
	m_controlPoints.push_back(glm::vec3(-345,-25, 590));
	m_controlPoints.push_back(glm::vec3(-345, -5, 400));
	//m_controlPoints.push_back(glm::vec3(-345, 50, 340));
	m_controlPoints.push_back(glm::vec3(-270, 5,240));
	m_controlPoints.push_back(glm::vec3(-180, 5,65));
	m_controlPoints.push_back(glm::vec3(-120, 5, 0));
	




	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(.3, .6, 0));
	m_controlUpVectors.push_back(glm::vec3(0, .6, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
	m_controlUpVectors.push_back(glm::vec3(0, 1, 0));
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size()-1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;
	
	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points

	SetControlPoints();


	// Call UniformlySampleControlPoints with the number of samples required

	UniformlySampleControlPoints(500);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1,&m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);

	// Create a VBO
	m_vboCL.Create();
	m_vboCL.Bind();
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	for (unsigned int i = 0; i < m_centrelinePoints.size(); i++) {
		m_vboCL.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
		m_vboCL.AddData(&texCoord, sizeof(glm::vec2));
		m_vboCL.AddData(&normal, sizeof(glm::vec3));
	}
	// Upload the VBO to the GPU
	m_vboCL.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

}


void CCatmullRom::CreateOffsetCurves(float width)
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
	glm::vec2 texCoord(0.0f, 0.0f);
	glm::vec3 normal(0.0f, 1.0f, 0.0f);
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	m_vboLL.Create();
	m_vboLL.Bind();
	//float width = 20.0f;
	for (int i = 0; i < m_centrelinePoints.size(); i++) {
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[(i+1) % m_centrelinePoints.size()];
		glm::vec3 up = m_controlUpVectors[(i + 1) % m_controlUpVectors.size()];
		normal = up;
		

		glm::vec3 t = glm::normalize(pNext - p);
		glm::vec3 n = glm::normalize(glm::cross(t, up));
		glm::vec3 b = glm::normalize(glm::cross(n, t));
		
		glm::vec3 l = p - (width / 2) * n;
		glm::vec3 r = p + (width / 2) * n;
		m_leftOffsetPoints.push_back(l);
		//m_rightOffsetPoints.push_back(r);

		normals.push_back(normal);
		texCoords.push_back(texCoord);


		m_vboLL.AddData(&l, sizeof(glm::vec3));
		m_vboLL.AddData(&texCoord, sizeof(glm::vec2));
		m_vboLL.AddData(&normal, sizeof(glm::vec3));

	}
	m_vboLL.UploadDataToGPU(GL_STATIC_DRAW);





	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Note it is possible to only use one VAO / VBO with all the points instead.
	
	
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));



	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);
	m_vboRL.Create();
	m_vboRL.Bind();

	for (int i = 0; i < m_centrelinePoints.size(); i++) {
		glm::vec3 p = m_centrelinePoints[i];
		glm::vec3 pNext = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()];



		glm::vec3 t = glm::normalize(pNext - p);
		glm::vec3 n = glm::normalize(glm::cross(t, glm::vec3(0, 1, 0)));
		glm::vec3 b = glm::normalize(glm::cross(n, t));
		
		glm::vec3 l = p - (width / 2) * n;
		glm::vec3 r = p + (width / 2) * n;
		m_rightOffsetPoints.push_back(r);



		m_vboRL.AddData(&r, sizeof(glm::vec3));
		m_vboRL.AddData(&texCoord, sizeof(glm::vec2));
		m_vboRL.AddData(&normal, sizeof(glm::vec3));

	}

	m_vboRL.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
		


}


void CCatmullRom::CreateTrack(string filename, int tiles)
{
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	m_vboT.Create();
	m_vboT.Bind();

	glm::vec2 texCords[4] = { glm::vec2(0,0), glm::vec2(1,0), glm::vec2(0,1), glm::vec2(1,1) };
	glm::vec3 up = { 0,1,0 };

	size_t totalPoints = m_rightOffsetPoints.size();


	for (int i = 0; i <= m_rightOffsetPoints.size(); i++) {

		float u = static_cast<float>(i) / static_cast<float>(totalPoints) * tiles;

		glm::vec2 texCoordsLeft(u, 0);
		glm::vec2 texCoordsRight(u, 1);

		m_vboT.AddData(&m_leftOffsetPoints.at(i% m_rightOffsetPoints.size()), sizeof(glm::vec3));
		m_vboT.AddData(&texCoordsLeft, sizeof(glm::vec2));
		m_vboT.AddData(&up, sizeof(glm::vec3));
		m_vertexCount += 1;

		m_vboT.AddData(&m_rightOffsetPoints.at(i % m_rightOffsetPoints.size()), sizeof(glm::vec3));
		m_vboT.AddData(&texCoordsRight, sizeof(glm::vec2));
		m_vboT.AddData(&up, sizeof(glm::vec3));
		m_vertexCount += 1;

	}
	m_vboT.UploadDataToGPU(GL_STATIC_DRAW);
	// Set the vertex attribute locations
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));



}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
	glPointSize(5);
	glLineWidth(2.5);
	glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size());

}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size());
	glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size());
	glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
}


void CCatmullRom::RenderTrack()
{
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Bind the VAO m_vaoTrack and render it
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	

}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);