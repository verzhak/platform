
#
# Параметры сборки:
#
# CMAKE_BUILD_TYPE=Debug или CMAKE_BUILD_TYPE=Release (по умолчанию)
#

cmake_minimum_required(VERSION 2.8)

project(platform)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Network REQUIRED)

pkg_check_modules(PC_AMV QUIET amv)

string(REPLACE ";" " " PC_AMV_CFLAGS " ${PC_AMV_CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} ${PC_AMV_CFLAGS} -Wall -pipe -std=gnu++11")
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)

	set(CMAKE_BUILD_TYPE Debug)

endif()

############################################################################ 

add_subdirectory(base)
add_subdirectory(main)
add_subdirectory(arinc)
add_subdirectory(card)

