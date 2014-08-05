
project(main)

add_executable(main main.cpp video.cpp process.cpp command.cpp)
qt5_use_modules(main Core Network Xml)
target_link_libraries(main base ${PC_AMV_LIBRARIES} ${OpenCV_LIBS})

add_subdirectory(modules)

