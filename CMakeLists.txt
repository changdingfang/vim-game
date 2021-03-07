cmake_minimum_required(VERSION 2.8)
project(vgame)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -lpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")

set(ROOT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXECUTABLE_OUTPUT_PATH ${ROOT_DIRECTORY}/build/bin)
set(LIBRARY_OUTPUT_PATH ${ROOT_DIRECTORY}/build/lib)

set(DEP_INCLUDE "${ROOT_DIRECTORY}/depend/compiled/include")
set(DEP_LIB "${ROOT_DIRECTORY}/depend/compiled/lib")

message(STATUS "DEP_INCLUDE: ${DEP_INCLUDE}")
message(STATUS "DEP_LIB: ${DEP_LIB}")

add_subdirectory(src)
