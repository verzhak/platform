
project(base)

add_library(base SHARED protocol.cpp socket.cpp)
target_link_libraries(base ${PC_AMV_LIBRARIES})

