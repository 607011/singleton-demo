cmake_minimum_required (VERSION 2.6)
project(loggerdemo)
set(CMAKE_CXX_FLAGS
  "-O2 -std=c++11")
set(HEADER_FILES
  logger.hpp)
add_executable(loggerdemo
	main.cpp
)
