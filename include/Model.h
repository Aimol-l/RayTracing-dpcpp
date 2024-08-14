#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include "Triangle.h"
#include "Material.h"

class Model{
private:
    std::vector<Triangle*> m_meshs;//网格面
    glm::mat4x4 m_trans; //模型矩阵
public:
    Model(const std::string &path,const Material mater);
    ~Model(){};
    size_t get_triangle_num(){
        return m_meshs.size();
    }
    std::vector<Triangle*>& get_meshs(){
        return m_meshs;
    }
};
