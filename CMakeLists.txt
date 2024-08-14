cmake_minimum_required(VERSION 3.20)
   
set(CMAKE_C_COMPILER /opt/intel/oneapi/compiler/latest/bin/icx)
set(CMAKE_CXX_COMPILER /opt/intel/oneapi/compiler/latest/bin/icpx)
set(IntelSYCL_DIR /opt/intel/oneapi/compiler/latest/lib/cmake/IntelSYCL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl -Wall -fsycl-targets=nvptx64-nvidia-cuda -Wc++20-extensions")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsycl -Wall -Wc++20-extensions")

# 获取 CMakeLists.txt 文件所在目录的绝对路径
get_filename_component(PROJECT_DIR ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_DIR}/bin)
set(CMAKE_BUILD_PARALLEL_LEVEL 8) 
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};/opt/intel/oneapi/mkl/latest/lib/cmake/mkl")

project(
    main        #项目名称
    LANGUAGES C CXX      #编程语言
    DESCRIPTION "光线跟踪demo"
)

file(GLOB SRC_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_subdirectory(3rdpart)

find_package(IntelSYCL REQUIRED)
# 包含MKL头文件目录
include_directories(${PROJECT_DIR}/include)
include_directories(${IntelSYCL_INCLUDE_DIRS})

add_executable(${CMAKE_PROJECT_NAME}  ${SRC_FILES})
add_sycl_to_target(TARGET ${CMAKE_PROJECT_NAME} SOURCES ${SRC_FILES})


target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC glm)
target_link_libraries(${CMAKE_PROJECT_NAME} ${IntelSYCL_LIBRARIES})

target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/3rdpart/glm)