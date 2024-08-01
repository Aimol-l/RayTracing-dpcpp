#pragma once
#include <iostream>

namespace ut{
    inline std::vector<std::string> split(const std::string& src,const char gap = ' '){
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(src);
        while (std::getline(iss, token, gap)) {
            tokens.push_back(token);
        }
        return std::move(tokens);
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





