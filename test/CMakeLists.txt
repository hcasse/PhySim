add_executable("reactive" "reactive.cpp")
target_link_libraries("reactive" "physim")

add_executable("composed_reactive" "composed_reactive.cpp")
target_link_libraries("composed_reactive" "physim")

add_executable("failing" "failing.cpp")
target_link_libraries("failing" "physim")

add_executable("periodic" "periodic")
target_link_libraries("periodic" "physim")
