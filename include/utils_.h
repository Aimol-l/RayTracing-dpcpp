#pragma once
#include <iostream>
#include <sstream>
#include "BVH.h"

// 说明：内联函数才可以在.h里面实现，否则需要在.cpp里面实现，不然会导致重复定义的问题
namespace ut{
    inline std::vector<std::string> split(const std::string& src,const char gap = ' '){
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(src);
        while (std::getline(iss, token, gap)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    inline bool startswith(const std::string& src, const std::string& start) {
        return src.compare(0, start.size(), start) == 0;
    }
    inline bool endswith(const std::string& src, const std::string& end) {
        return src.compare(src.size() - end.size(), end.size(), end) == 0;
    }
    template<typename ...U>
    void println(U... u){
        int i = 0;
        auto printer = [&i]<typename Arg>(Arg arg){
            if(sizeof...(U) == ++i) std::cout<<arg<<std::endl;
            else std::cout<<arg<<" ";
        };
        (printer(u),...);
    }
} 





