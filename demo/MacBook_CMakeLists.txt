project(NCNN_INFER_DEMO)
cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_BUILD_TYPE Debug)

# 设置 OpenMP 相关的变量
set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
set(OpenMP_C_LIB_NAMES "omp")
set(OpenMP_CXX_LIB_NAMES "omp")
set(OpenMP_omp_LIBRARY "/opt/homebrew/Cellar/libomp/18.1.5/lib/libomp.dylib")
# 设置OpenMP 头文件搜索路径
include_directories("/opt/homebrew/Cellar/libomp/18.1.5/include")

find_package(ncnn REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenMP REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(demo src/demo.cpp)
target_link_libraries(demo ${OpenCV_LIBS} ncnn)

#如果找到 OpenMP库， 则添加链接
if(OpenMP_CXX_FOUND)
    target_link_libraries(demo OpenMP::OpenMP_CXX)
else()
    message("OpenMP not found")
endif()
