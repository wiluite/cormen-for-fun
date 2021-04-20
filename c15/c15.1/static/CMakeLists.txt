cmake_minimum_required(VERSION 3.17)
project(ch15_4)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "You would rather use a Clang compiler that is sufficiently fast at heavy metaprogramming tasks")
else()
    set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "  ")
endif()

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.65)
include_directories(${Boost_INCLUDE_DIRS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    #target_link_libraries(ch15_4 -stdlib=libc++)
endif()
add_executable(ch15_4 main.cpp )
