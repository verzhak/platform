
project(contour)

set(sources main.cpp)

add_executable(contour ${sources})
target_link_libraries(contour amv ${OpenCV_LIBS} ${PC_AMV_LIBRARIES})

