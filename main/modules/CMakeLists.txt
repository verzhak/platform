
cmake_minimum_required(VERSION 2.8)

project(modules)

find_package(OpenCV REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pipe -std=gnu++11")
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_library(contour SHARED contour/bits.cpp contour/preprocess.cpp contour/morphology.cpp contour/skeleton.cpp contour/gabor.cpp contour/approx.cpp contour/ann.cpp contour/simple_neuro.cpp contour/base_cnn.cpp contour/sobel.cpp contour/my_neuro.cpp)
target_link_libraries(contour ${OpenCV_LIBS})

############################################################################ 

add_library(gabor SHARED gabor.cpp)
target_link_libraries(gabor ${OpenCV_LIBS} contour)

add_executable(gabor_test main.cpp gabor.cpp)
target_link_libraries(gabor_test ${OpenCV_LIBS} contour)

############################################################################ 

add_library(bits SHARED bits.cpp)
target_link_libraries(bits ${OpenCV_LIBS} contour)

add_executable(bits_test main.cpp bits.cpp)
target_link_libraries(bits_test ${OpenCV_LIBS} contour)

############################################################################ 

add_library(ann SHARED ann.cpp)
target_link_libraries(ann ${OpenCV_LIBS} contour)

add_executable(ann_test main.cpp ann.cpp)
target_link_libraries(ann_test ${OpenCV_LIBS} contour)

############################################################################ 

add_library(morph SHARED morph.cpp)
target_link_libraries(morph ${OpenCV_LIBS} contour)

add_executable(morph_test main.cpp morph.cpp)
target_link_libraries(morph_test ${OpenCV_LIBS} contour)

############################################################################ 

add_library(sobel SHARED sobel.cpp)
target_link_libraries(sobel ${OpenCV_LIBS} contour)

add_executable(sobel_test main.cpp sobel.cpp)
target_link_libraries(sobel_test ${OpenCV_LIBS} contour)

############################################################################ 

add_library(german SHARED german.cpp)
target_link_libraries(german ${OpenCV_LIBS})

add_executable(german_test main.cpp german.cpp)
target_link_libraries(german_test ${OpenCV_LIBS})

