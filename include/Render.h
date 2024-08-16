#pragma once
#include "utils_.h"
#include <CL/sycl.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

sycl::queue init_queue(){
    sycl::queue queue(sycl::default_selector_v); //手动指定GPU设备
    // sycl::queue queue(sycl::cpu_selector_v); //手动指定CPU设备
    auto sycl_device = queue.get_device();
    auto sycl_context = queue.get_context();
    std::cout<<"*************************device info************************"<<std::endl;
    sycl::info::device_type info = queue.get_device().get_info<sycl::info::device::device_type>();
    if(sycl::info::device_type::cpu == info) std::cout << "设备类型: CPU" << std::endl;
    if(sycl::info::device_type::gpu == info) std::cout << "设备类型: GPU" << std::endl;
    if(sycl::info::device_type::accelerator == info) std::cout << "设备类型: ???" << std::endl;
    std::cout<<"设备名称："<< queue.get_device().get_info<sycl::info::device::name>()<<std::endl;
    std::cout<<"最大工作小组大小:"<<queue.get_device().get_info<sycl::info::device::max_work_group_size>()<<std::endl;
    std::cout<<"全局内存大小: "<< queue.get_device().get_info<sycl::info::device::global_mem_size>()/float(1073741824) << " Gbytes"<<std::endl;
    std::cout<<"最大计算单元数量: "<< queue.get_device().get_info<sycl::info::device::max_compute_units>()<<std::endl;
    std::cout<<"*************************************************************"<<std::endl;
    return queue;
}

struct Ray{
    sycl::float3 origin;
    sycl::float3 direction;
};
struct HitResult{
    bool is_hit = false; // 是否击中
    float distance = -1;     //起点和击中点之间的距离
    sycl::float3 emit;  // 是否为发光
    sycl::float3 hitpoint; //击中点
    sycl::float3 normal;    //击中点所在三角面的法线
};
const static int W = 250; // 图像宽度
const static int H = 250;  // 图像高度
inline float randf(int seed) {
    float r = float((1664525 * seed + 1013904223) % 4294967296)/4294967296;
    return r;
}
inline sycl::float3 normalize(float x,float y,float z){
    float l2 = sycl::sqrt(x*x+y*y+z*z);
    return {x/l2,y/l2,z/l2};
}
// 有一个交点说明射线起点在bbox内部
// 有两个交点说明射线起点在bbox外部
// 没有交点返回-1
float hitAABB(Ray &ray,sycl::float3 min_xyz,sycl::float3 max_xyz){
    sycl::float3 invdir = 1.0f / ray.direction;
    sycl::float3 f = (max_xyz - ray.origin)*invdir;
    sycl::float3 n = (min_xyz - ray.origin)*invdir;
    sycl::float3 tmax = sycl::max(f, n);
    sycl::float3 tmin = sycl::min(f, n);
    float t1 = sycl::min(tmax.x(), sycl::min(tmax.y(), tmax.z()));
    float t0 =  sycl::max(tmin.x(), sycl::max(tmin.y(), tmin.z()));
    return (t1 >= t0) ? ((t0 > 0.0) ? (t0) : (t1)) : (-1);
}
HitResult hitBVH(Ray &ray,BVHNode* bvh_data){
    HitResult res;
    // 遍历bvh树，找到ray与三角面的交点
    int stack[256];
    int top = 0;
    stack[0] = 0;
    while (top>=0){
        // stack顶端是叶子节点，就继续内部的三角形是否与射线相交(大概率会)
        if(bvh_data[stack[top]].is_leaf){
            // 判断相交，保存交点，距离，面法线，发光情况
            res.is_hit = true;
            // 计算交点
            res.distance = 1.0f;
            // 计算交点与射线的距离

            // 计算面法线
            break;
        }
        // 计算左右节点与ray的交，交点距离远的先push到stack内
        auto left = bvh_data[stack[top]].left_index;
        auto right = bvh_data[stack[top]].right_index;
        stack[top--] = 0;
        float d1 = -FLT_MAX; // 左盒子交点距离
        float d2 = -FLT_MAX; // 右盒子交点距离
        d1 = hitAABB(ray,bvh_data[left].min_xyz,bvh_data[left].max_xyz);
        d2 = hitAABB(ray,bvh_data[right].min_xyz,bvh_data[right].max_xyz);
        if(d1>0 && d2>0) {
            if(d1<d2){
                stack[++top] = right;
                stack[++top] = left;
            }else{
                stack[++top] = left;
                stack[++top] = right;
            }
        }else if(d1>0){
            stack[++top] = left;
        }else if(d2>0){
            stack[++top] = right;
        }
    }
    return res;
}
sycl::float3 trace_ray(Ray ray,BVHNode* bvh_data){
    // 需要利用bvh树判断ray是否和bbox相交
    HitResult hitresult = hitBVH(ray,bvh_data);
    sycl::float3 hitcolor{0,0,0};
    if(!hitresult.is_hit) return hitcolor;
    // 如果ray打中了三角形，那么需要进行反射
    return sycl::float3{hitresult.distance,hitresult.distance,hitresult.distance};
}
void render(std::vector<BVHNode> &bvhtree, int spp=1){
    sycl::queue queue = init_queue();
    std::array<sycl::float3, W * H> image; // 创建绘制画布
    BVHNode* bvh_data = sycl::malloc_device<BVHNode>(bvhtree.size(),queue); //只在gpu上用
    sycl::float3* image_data = sycl::malloc_shared<sycl::float3>(image.size(),queue);//host 和 device上都要用
    // 将数据从host复制到device
    queue.memcpy(bvh_data, bvhtree.data(), bvhtree.size() * sizeof(BVHNode)).wait();
    queue.memcpy(image_data, image.data(), image.size() * sizeof(sycl::float3)).wait();
    // 核函数
    queue.submit([&](sycl::handler &h){
        h.parallel_for(sycl::range<2>(W, H),[=](sycl::id<2> idx){
            // 先将x和y从像素坐标系变换到opengl坐标系
            float x = 2.0 * float(idx[1]) / float(W) - 1.0;
            float y = 2.0 * float(H - idx[0]) / float(H) - 1.0;
            sycl::float3 color = {0,0,0};
            sycl::float3 eye_pos = {0,0,5}; //眼睛的位置
            // for(int i=0;i<spp;i++){
                // MSAA，在像素点范围内再次随机取样
                // x += (randf(i*idx[1]-idx[0]) - 0.5f) / float(W);
                // y += (randf(i*idx[1]+idx[0]) - 0.5f) / float(H);
                sycl::float3 coord = normalize(x,y,1.0f); //视平面上采样点位置
                sycl::float3 ray_dir = normalize(coord - eye_pos); //光线的方向，实际反射的时候是用逆方向
                Ray ray{eye_pos,ray_dir};
                // // 路径追踪
                color = trace_ray(ray,bvh_data);
            // }
            image_data[idx[0] * W + idx[1]] = color;
        });
    }).wait(); 

    //显示结果
    
    for(size_t h=0;h<H;h++){
        for(size_t w =0;w<W;w++){
            std::cout<<image_data[h*W+w].y()<<",";
        }
        // std::cout<<"\n";
    } 
    // 释放USM内存
    sycl::free(bvh_data, queue);
    sycl::free(image_data, queue);
}
