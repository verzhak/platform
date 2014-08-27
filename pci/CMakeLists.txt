
project(pci)

add_library(pci SHARED pci.cpp)
target_link_libraries(pci base ${PC_AMV_LIBRARIES})

