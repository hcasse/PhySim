set(SOURCES
	"Model.cpp"
	"Monitor.cpp"
	"Port.cpp"
	"Simulation.cpp"
	"std.cpp"
	"test.cpp"
	"Value.cpp"
)
add_library(physim STATIC ${SOURCES})

if(NOT NO_QT)
	include_directories(${Qt5Charts_INCLUDE_DIRS})
	set(QT_SOURCES "qtcharts.cpp")
	QT5_WRAP_CPP(QT_SOURCES "physim_qt.h" )
	add_library(physim_qt STATIC ${QT_SOURCES})
endif()

