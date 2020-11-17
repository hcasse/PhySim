set(SOURCES
	"Model.cpp"
	"Monitor.cpp"
	"Port.cpp"
	"Simulation.cpp"
	"std.cpp"
	"test.cpp"
	"Value.cpp"
)

if(NOT NO_QT)
	include_directories(${Qt5Charts_INCLUDE_DIRS})
	list(APPEND SOURCES "qtcharts.cpp")
endif()

add_library(physim STATIC ${SOURCES})
