cmake_minimum_required(VERSION 3.3)

## define our project
project(loranger_gateway)

## request C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wformat=0")


include_directories(${PROJECT_SOURCE_DIR}/include)

set(BUILD_TESTS OFF CACHE BOOL "Build Tests")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libraries")
#set(Boost_USE_STATIC_LIBS ON FORCE)

add_subdirectory("crocore")
include_directories(${crocore_INCLUDE_DIRS})
set(LIBS ${LIBS} ${crocore_LIBRARIES})


add_subdirectory("src")

