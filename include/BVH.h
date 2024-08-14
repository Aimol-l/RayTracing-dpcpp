#pragma once
#include <array>
#include <vector>
#include <CL/sycl.hpp>
#include "Triangle.h"

struct BVHNode{
    bool is_leaf = true;
    sycl::float3 min_xyz = {FLT_MAX, FLT_MAX, FLT_MAX};
    sycl::float3 max_xyz = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
    size_t left_index = 0;
    size_t right_index = 0;
    Triangle trangle;
};
struct Bounds{
    sycl::float3 min_xyz = {FLT_MAX, FLT_MAX, FLT_MAX};
    sycl::float3 max_xyz = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
};

// 求出bbox 和 三角形的最小共同bbox
auto union_box(Bounds bbox, std::array<sycl::float3, 3> &points);
// 计算最长轴
int max_axis(Bounds & bbox);
void build(std::vector<BVHNode>& tree,std::vector<Triangle *>& tris);
std::vector<BVHNode> BVHCreate(std::vector<Triangle *>& tris);