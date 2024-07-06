cmake_minimum_required(VERSION 3.0.0)
project(fancyarray VERSION 0.1.0)

# enable_testing()

add_executable(fancyarray main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 20)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

