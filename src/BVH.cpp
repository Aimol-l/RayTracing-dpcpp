#pragma once
#include "BVH.h"
#include <algorithm>
// 求出bbox 和 三角形的最小共同bbox
auto union_box(Bounds bbox, std::array<sycl::float3, 3> &points) {
    // 更新bbox的最小和最大坐标
    for (const auto &point : points) {
        bbox.min_xyz = sycl::min(bbox.min_xyz, point);
        bbox.max_xyz = sycl::max(bbox.max_xyz, point);
    }
    return bbox;
}
// 计算最长轴
int max_axis(Bounds & bbox){
    sycl::float3 out = bbox.max_xyz - bbox.min_xyz;
    if(out.x() >= out.y() && out.x() >= out.z()) return 0; // x axis
    if(out.y() >= out.x() && out.y() >= out.z()) return 1; // y axis
    if(out.z() >= out.y() && out.z() >= out.x()) return 2; // z axis
    return -1;
}
void build(std::vector<BVHNode>& tree,std::vector<Triangle *>& tris){
    BVHNode node;
    Bounds bounds;
    for (int i = 0; i < tris.size(); ++i){
        sycl::float3 a = {tris[i]->vertexs[0].x,tris[i]->vertexs[0].y,tris[i]->vertexs[0].z};
        sycl::float3 b = {tris[i]->vertexs[1].x,tris[i]->vertexs[1].y,tris[i]->vertexs[1].z};
        sycl::float3 c = {tris[i]->vertexs[2].x,tris[i]->vertexs[2].y,tris[i]->vertexs[2].z};
        std::array<sycl::float3,3> points = {a,b,c};
        bounds = union_box(bounds,points);
    }
    node.min_xyz = bounds.min_xyz;
    node.max_xyz = bounds.max_xyz;
    if(tris.size() == 1){
        // 叶子节点
        node.is_leaf = true;
        node.trangle = *tris[0]; //要拷贝构造出新的对象
        tree.push_back(std::move(node));
        return;
    }else{
        // 非叶子节点
        node.is_leaf = false;
        // 对 tris 进行分割
        auto dim = max_axis(bounds);
        if(dim == 0){
            std::sort(tris.begin(),tris.end(),[](Triangle* t1,Triangle* t2){
                glm::vec3 c1 = (t1->vertexs[0] + t1->vertexs[1] + t1->vertexs[2])/ glm::vec3(3);
                glm::vec3 c2 = (t2->vertexs[0] + t2->vertexs[1] + t2->vertexs[2])/ glm::vec3(3);
                return c1.x < c2.x;
            });
        }else if(dim == 1){
            std::sort(tris.begin(),tris.end(),[](Triangle* t1,Triangle* t2){
                glm::vec3 c1 = (t1->vertexs[0] + t1->vertexs[1] + t1->vertexs[2])/ glm::vec3(3);
                glm::vec3 c2 = (t2->vertexs[0] + t2->vertexs[1] + t2->vertexs[2])/ glm::vec3(3);
                return c1.y < c2.y;
            });
        }else if(dim == 2){
            std::sort(tris.begin(),tris.end(),[](Triangle* t1,Triangle* t2){
                glm::vec3 c1 = (t1->vertexs[0] + t1->vertexs[1] + t1->vertexs[2])/ glm::vec3(3);
                glm::vec3 c2 = (t2->vertexs[0] + t2->vertexs[1] + t2->vertexs[2])/ glm::vec3(3);
                return c1.z < c2.z;
            });
        }
        auto beginning = tris.begin();
        auto middling = tris.begin() + (tris.size() / 2);
        auto ending = tris.end();

        auto leftshapes = std::vector<Triangle*>(beginning, middling);
        auto rightshapes = std::vector<Triangle*>(middling, ending);

        assert(tris.size() == (leftshapes.size() + rightshapes.size()));

        size_t curr_node_index = tree.size();
        node.left_index = curr_node_index + 1;
        tree.push_back(node);
        build(tree,leftshapes);
        tree.at(curr_node_index).right_index = tree.size();
        build(tree,rightshapes);
    }
}
std::vector<BVHNode> BVHCreate(std::vector<Triangle *>& tris){
    std::vector<BVHNode> tree;
    build(tree,tris);
    return tree;
}