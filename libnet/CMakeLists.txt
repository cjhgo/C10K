cmake_minimum_required(VERSION 2.6)


project(libnet C CXX)

set(CXX_FLAGS
  -std=c++11
)

set (CMAKE_CXX_STANDARD 11)


MESSAGE( "INCLUDE " ${libnet_SOURCE_DIR})
include_directories(${libnet_SOURCE_DIR})

add_subdirectory(libnet)

macro(add_libnet_executable NAME)
  add_executable(${NAME} ${ARGN})
  target_link_libraries(${NAME} libnet)
endmacro(add_libnet_executable)


add_subdirectory(examples)

