
project(base)

add_library(base SHARED config.cpp)
qt5_use_modules(base Core Xml)
target_link_libraries(base ${PC_AMV_LIBRARIES})

