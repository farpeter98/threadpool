cmake_minimum_required(VERSION 3.0.0)
project(threadpool VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

include_directories(test PRIVATE includes)

add_executable(test main.cpp includes/PooledThread.hpp includes/TaskEntry.hpp includes/ThreadDispatcher.hpp)
