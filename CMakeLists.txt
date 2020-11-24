cmake_minimum_required(VERSION 3.0)

project(PhySim)

# uncomment to remove Qt use
#set(NO_QT ON)

if(NOT NO_QT)
	#set(CMAKE_AUTOMOC ON)
	#set(CMAKE_AUTORCC ON)
	#set(CMAKE_AUTOUIC ON)
	find_package(Qt5 COMPONENTS Widgets REQUIRED)
	find_package(Qt5 COMPONENTS Charts REQUIRED)

	set(QT_LIBS		Qt5::Widgets Qt5::Charts)
	add_definitions(${Qt5Widgets_DEFINITIONS} -fPIC)
	include_directories(${Qt5Widgets_INCLUDE_DIRS})
endif()

include_directories("include")

add_subdirectory("src")
add_subdirectory("test")
add_subdirectory("UE33")
add_subdirectory("UE34 - sol")

add_executable("sim" "main.cpp")
target_link_libraries("sim" ${MY_LIBS})
