# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(matplot_example VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找OpenGL和GLFW
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# 添加头文件目录
include_directories(${OPENGL_INCLUDE_DIR}
    ${GLFW3_INCLUDE_DIR})

add_executable(${PROJECT_NAME} main.cpp)
add_executable(oscilloscope osc.cpp)
# 链接库
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    glfw)
target_link_libraries(oscilloscope
    ${OPENGL_LIBRARIES}
    glfw)
