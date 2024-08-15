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
    // Model floor("../assets/cornellbox/floor.obj",MATE);
    // Model left("../assets/cornellbox/left.obj",MATE);
    // Model light("../assets/cornellbox/light.obj",MATE);
    // Model right("../assets/cornellbox/right.obj",MATE);
    // Model shortbox("../assets/cornellbox/shortbox.obj",MATE);
    // Model tallbox("../assets/cornellbox/tallbox.obj",MATE);

    Model cube("../assets/123456.obj",MATE);
    Model moon("../assets/moon.obj",MATE);
    Scenes scenes;
    scenes.add_model(cube);
    // scenes.add_model(floor,left,light,right,shortbox,tallbox);
    auto triangles = scenes.get_triangles();
    ut::println("三角形数量",triangles.size());
    //创建线性bvh树
    auto bvhtree = BVHCreate(triangles); 
    int i =0;
    for(auto const & node:bvhtree){
        ut::println("idx:",i++,node.is_leaf);
        ut::println(node.left_index,node.right_index);
    }
    ut::println("bvh长度",bvhtree.size());
    // 渲染
    render(bvhtree,10);
    // done
    return 0;
}
