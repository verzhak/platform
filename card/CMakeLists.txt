
project(card)

set(CMAKE_AUTOMOC ON)

set(sources main_loop.cpp)

add_executable(card_x86_64 main_x86_64.cpp ${sources})
qt5_use_modules(card_x86_64 Core Network Xml)
target_link_libraries(card_x86_64 base ${PC_AMV_LIBRARIES})

add_executable(card_mips main_mips.cpp ${sources})
target_link_libraries(card_mips base ${PC_AMV_LIBRARIES})

