cmake_minimum_required(VERSION 3.0)

project(PhySim)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Charts REQUIRED)

set(MY_LIBS
	"physim"
	Qt5::Widgets
	Qt5::Charts)

include_directories("include")
add_definitions(${Qt5Widgets_DEFINITIONS} -fPIC)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

add_subdirectory("src")
add_subdirectory("test")

add_executable("sim" "main.cpp")
target_link_libraries("sim" ${MY_LIBS})

