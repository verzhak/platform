
project(arinc)

set(CMAKE_AUTOMOC ON)

add_executable(arinc main.cpp main_loop.cpp ../main/socket.cpp)
qt5_use_modules(arinc Core Network Xml)
target_link_libraries(arinc base ${PC_AMV_LIBRARIES} ${OpenCV_LIBS})

