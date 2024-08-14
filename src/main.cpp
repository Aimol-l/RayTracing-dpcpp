#include <iostream>
#include "utils_.h"
#include "Scenes.h"
#include "BVH.h"
#include "Render.h"


int main(int argc, char const *argv[]){
    // 创建场景，获取出所有三角形
    Material MATE{
        .emissive = {1,1,1},
        .base_color = {0,0.8,0.6},
        .specular = 0,
        .diffuse = 0.4
    };
    Model cube("../assets/123456.obj",MATE);
    Model moon("../assets/moon.obj",MATE);
    Scenes scenes;
    scenes.add_model(cube,moon);
    auto triangles = scenes.get_triangles();
    ut::println("三角形数量",triangles.size());
    //创建线性bvh树
    auto bvhtree = BVHCreate(triangles); 
    ut::println("bvh长度",bvhtree.size());
    // 渲染
    render(bvhtree,10);
    // done
    return 0;
}
