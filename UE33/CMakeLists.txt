add_executable("pid" "pid.cpp")
if(NOT NO_QT)
	target_link_libraries("pid" "physim_qt" "physim" ${QT_LIBS})
else()
	target_link_libraries("pid" "physim")
endif()
