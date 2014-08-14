
project(card)

add_executable(card main.cpp main_loop.cpp map.cpp socket.cpp matrix.cpp)
target_link_libraries(card m base pci ${PC_AMV_LIBRARIES} ${PC_SXF_LIBRARIES})

