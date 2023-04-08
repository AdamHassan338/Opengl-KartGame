#include "Obstacle.h"

Obstacle::Obstacle() 
{
	
}

Obstacle::~Obstacle()
{
}





void Obstacle::set(float distance, int offset, CCatmullRom* path)
{
	
	glm::vec3 up;
	path->Sample(distance, m_pos, up);

	glm::vec3 pNext;
	path->Sample(distance + 1.0f, pNext);

	glm::vec3 T = glm::normalize(pNext - m_pos);

	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0)));
	glm::vec3 B = glm::normalize(glm::cross(N, T));
	m_pos += N * (float)offset;

	m_rotation = glm::mat4(glm::mat3(T, B, N));


}
