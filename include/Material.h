#pragma once
#include "glm/glm.hpp"

struct Material{
    glm::vec3 emissive{0,0,0};  // 自发光参数
    glm::vec3 base_color{1,1,1}; // 固有色
    float specular = 0; // 镜面反射
    float diffuse = 0;  // 漫反射
};
