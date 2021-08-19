cmake_minimum_required(VERSION 3.20)
project(btts)

set(CMAKE_CXX_STANDARD 17)

add_executable(btts main.cpp main.h utils.h BinaryTreeTestingSimulation.cpp BinaryTreeTestingSimulation.h)
