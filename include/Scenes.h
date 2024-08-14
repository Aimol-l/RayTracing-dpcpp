#pragma once
#include <vector>
#include <string>
#include <memory>
#include "glm/glm.hpp"
#include "Model.h"

class Scenes{
private:
    std::vector<std::shared_ptr<Model>> m_models;
public:
    Scenes(){};
    ~Scenes(){};
    // 使用可变参数模板添加模型
    template <typename... Args>
    void add_model(Args&&... args) {
        // 使用折叠表达式展开参数包，并创建每个模型
        ([&](auto&& arg) {
            m_models.emplace_back(
                std::make_shared<std::decay_t<decltype(arg)>>(std::forward<decltype(arg)>(arg))
            );
        }(args), ...);
    }
    std::vector<Triangle*> get_triangles(){
        std::vector<Triangle*> t;
         for(auto & model:m_models){
            for(auto* triangle:model->get_meshs()){
                t.push_back(triangle);
            }
         }
        return t;
    }
};