set(SOURCES
	"Model.cpp"
	"Monitor.cpp"
	"Port.cpp"
	"qtcharts.cpp"
	"Simulation.cpp"
	"std.cpp"
	"test.cpp"
)

include_directories(${Qt5Charts_INCLUDE_DIRS})
add_library(physim STATIC ${SOURCES})
