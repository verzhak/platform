
project(card)

pkg_check_modules(PC_COMBINE QUIET combine)
include_directories(${PC_COMBINE_INCLUDE_DIRS})

add_executable(card main.cpp main_loop.cpp map.cpp pci.cpp)
target_link_libraries(card base ${PC_AMV_LIBRARIES} ${PC_COMBINE_LIBRARIES} glapi ${OpenCV_LIBS})

