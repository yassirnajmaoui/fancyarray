cmake_minimum_required(VERSION 3.0.0)
project(fancyarray VERSION 0.1.0)

add_executable(test_fancyarray test_fancyarray.cpp)

add_executable(performance_tuple_of_vectors performance_tuple_of_vectors.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 20)


