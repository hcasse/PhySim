add_executable("reactive" "reactive.cpp")
target_link_libraries("reactive" "physim")

add_executable("composed_reactive" "composed_reactive.cpp")
target_link_libraries("composed_reactive" "physim")

add_executable("failing" "failing.cpp")
target_link_libraries("failing" "physim")

add_executable("periodic" "periodic.cpp")
target_link_libraries("periodic" "physim")

add_executable("constant_display" "constant_display.cpp")
target_link_libraries("constant_display" "physim")

add_executable("reporter" "reporter.cpp")
target_link_libraries("reporter" "physim")

if(NOT NO_QT)
	add_executable("qtline" "qtline.cpp")
	target_link_libraries("qtline" "physim_qt" "physim" ${QT_LIBS})
	
	add_executable("qtsim" "qtsim.cpp")
	target_link_libraries("qtsim" "physim_qt" "physim" ${QT_LIBS})
endif()

add_executable("pingpong" "pingpong.cpp")
target_link_libraries("pingpong" "physim")
