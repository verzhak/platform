
project(pci)

add_library(pci SHARED pci.cpp)
target_link_libraries(pci base ${PC_AMV_LIBRARIES})

add_executable(pci_bus main.cpp)
target_link_libraries(pci_bus base pci ${PC_AMV_LIBRARIES})

