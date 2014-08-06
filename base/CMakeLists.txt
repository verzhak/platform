
project(base)

add_library(base SHARED config.cpp protocol.cpp)
target_link_libraries(base ${PC_AMV_LIBRARIES})

