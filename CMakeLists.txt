cmake_minimum_required(VERSION 3.0.0)
project(threadpool VERSION 0.1.0)

set(EXEC ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

include_directories(includes)

add_executable(${EXEC} main.cpp)
