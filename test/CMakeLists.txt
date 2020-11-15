add_executable("reactive" "reactive.cpp")
target_link_libraries("reactive" ${MY_LIBS})

add_executable("composed_reactive" "composed_reactive.cpp")
target_link_libraries("composed_reactive" ${MY_LIBS})

add_executable("failing" "failing.cpp")
target_link_libraries("failing" ${MY_LIBS})

add_executable("periodic" "periodic.cpp")
target_link_libraries("periodic" ${MY_LIBS})

add_executable("constant_display" "constant_display.cpp")
target_link_libraries("constant_display" ${MY_LIBS})

add_executable("reporter" "reporter.cpp")
target_link_libraries("reporter" ${MY_LIBS})

if(NOT NO_QT)
	add_executable("qtline" "qtline.cpp")
	target_link_libraries("qtline" ${MY_LIBS})
endif()
