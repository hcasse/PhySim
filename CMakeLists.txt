cmake_minimum_required(VERSION 3.0)

project(Sat)

set(SOURCES
	"src/Model.cpp"
	"src/Monitor.cpp"
	"src/Port.cpp"
	"src/Simulation.cpp"
	"main.cpp"
)

include_directories("include")
add_executable(sat ${SOURCES})
