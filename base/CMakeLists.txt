
project(base)

add_library(base SHARED tree.cpp protocol.cpp socket.cpp)
target_link_libraries(base ${PC_AMV_LIBRARIES})

