#include "Model.h"
#include "utils_.h"
Model::Model(const std::string &path,const Material mater){
    std::ifstream inputFile(path);
    if(!inputFile.is_open()) {
        return;
    }
    std::string line;
    std::vector<glm::vec3> vertexes,normals;
    while(std::getline(inputFile, line)) {
        if(line.substr(0,2) == "v "){
            std::istringstream substr(line.substr(2));
            glm::vec3 v;
            substr >>v.x>>v.y>>v.z;
            vertexes.push_back(v);
        }
        if(line.substr(0,2) == "vn"){
            std::istringstream substr(line.substr(2));
            glm::vec3 vn;
            substr >>vn.x>>vn.y>>vn.z;
            normals.push_back(vn);
        }
        if(line.substr(0,2) == "f "){
            std::vector<std::string> results = ut::split(line.substr(2));
            // results.erase(std::remove(results.begin(), results.end(), ""), results.end()); // c++23
            // results.erase(std::remove(results.begin(), results.end(), "\r"), results.end()); // c++23
            // 移除所有空字符串 17
            results.erase(std::remove_if(results.begin(), results.end(), [](const std::string& str) { return str.empty(); }), results.end());
            // 移除所有包含换行符的字符串 17
            results.erase(std::remove_if(results.begin(), results.end(), [](const std::string& str) { return str.find('\r') != std::string::npos; }), results.end());
            if(results.size() == 3){ // 1/1/1,5/2/1,7/3/1
                Triangle *f1 = new Triangle();
                auto part1 = ut::split(results[0],'/');
                auto part2 = ut::split(results[1],'/');
                auto part3 = ut::split(results[2],'/');
                f1->vertexs[0] = vertexes.at(std::stoi(part1[0])-1);
                f1->vertexs[1] = vertexes.at(std::stoi(part2[0])-1);
                f1->vertexs[2] = vertexes.at(std::stoi(part3[0])-1);
                f1->n = normals.at(std::stoi(part1[2])-1);
                f1->base_color = mater.base_color;
                f1->diffuse = mater.diffuse;
                f1->emissive = mater.emissive;
                f1->specular = mater.specular;
                m_meshs.push_back(f1);
            }else if(results.size() == 4){ //f 1/1/1 5/2/1 7/3/1 3/4/1
                Triangle *f1=new Triangle();;
                Triangle *f2=new Triangle();;
                auto part1 = ut::split(results[0],'/');
                auto part2 = ut::split(results[1],'/');
                auto part3 = ut::split(results[2],'/');
                auto part4 = ut::split(results[3],'/');
                // std::println("{},{},{},{},n={}",part1[0],part2[0],part3[0],part4[0],part4[2]);
                f1->vertexs[0] = vertexes.at(std::stoi(part1[0])-1);
                f1->vertexs[1] = vertexes.at(std::stoi(part2[0])-1);
                f1->vertexs[2] = vertexes.at(std::stoi(part3[0])-1);
                f1->n = normals.at(std::stoi(part1[2])-1);
                f1->base_color = mater.base_color;
                f1->diffuse = mater.diffuse;
                f1->emissive = mater.emissive;
                f1->specular = mater.specular;

                f2->vertexs[0] = vertexes.at(std::stoi(part2[0])-1);
                f2->vertexs[1] = vertexes.at(std::stoi(part3[0])-1);
                f2->vertexs[2] = vertexes.at(std::stoi(part4[0])-1);
                f2->n = normals.at(std::stoi(part2[2])-1);
                f2->base_color = mater.base_color;
                f2->diffuse = mater.diffuse;
                f2->emissive = mater.emissive;
                f2->specular = mater.specular;
                
                m_meshs.push_back(f1);
                m_meshs.push_back(f2);
            }
        }
    }
    inputFile.close();
}
