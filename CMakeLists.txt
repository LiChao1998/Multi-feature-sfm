cmake_minimum_required(VERSION 3.10)

SET(CMAKE_BUILD_TYPE "Debug")

project(sfm)

find_package(OpenCV REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem)

if(OpenCV_FOUND)
    message(STATUS "opencv has been found: " ${OpenCV_INCLUDE_DIRS})
endif()

include_directories(${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/srcs)

FILE(GLOB SRCS ${CMAKE_SOURCE_DIR}/srcs/*.cpp)
add_library(sfm_utils SHARED ${SRCS})

add_executable(sfm main.cpp)

target_link_libraries(sfm sfm_utils ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})

