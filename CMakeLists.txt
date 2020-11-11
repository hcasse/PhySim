cmake_minimum_required(VERSION 3.0)

project(PhySim)

include_directories("include")

add_subdirectory("src")
add_subdirectory("test")

add_executable("sim" "main.cpp")
target_link_libraries("sim" "physim")

