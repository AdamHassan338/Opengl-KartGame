#include "Obstacle.h"

Obstacle::Obstacle() 
{
	
}





void Obstacle::set(float distance, CCatmullRom* path)
{
	
	glm::vec3 up;
	path->Sample(distance, m_pos, up);

	glm::vec3 pNext;
	path->Sample(distance, m_pos);
	path->Sample(distance + 1.0f, pNext);

	glm::vec3 T = glm::normalize(pNext - m_pos);

	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));

	//float w = 40.0f;


	//glm::vec3 l = p - (w / 2) * N;
	//glm::vec3 r = p + (w / 2) * N;

	// Cart update
	//glm::vec3 up = glm::rotate(glm::vec3(0, 1, 0), m_cameraRotation, T);

	//m_kartPos = glm::vec3(p.x, p.y - 1, p.z);
	m_rotation = glm::mat4(glm::mat3(T, B, N));


}
