#pragma once

#include "CatmullRom.h"
#include "MyObject.h"
class Obstacle : public MyObject
    

{
    
public:
    Obstacle();
    ~Obstacle();
    //void initalize(string a_sDirectory, string a_sFilename);

    void set(float distance, CCatmullRom* path);

    glm::vec3 m_pos;
    glm::mat4 m_rotation;
};

