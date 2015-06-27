cmake_minimum_required(VERSION 2.8)

project(cps3_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.49.0 EXACT REQUIRED)

include_directories(../../include ${Boost_INCLUDE_DIR})

set(CPS3_SRCS
	src/BasicIOTest.hpp
	src/BasicIOTest.cpp
	src/main.cpp
	src/SamplesTest.hpp
	src/SamplesTest.cpp
	src/TestCase.hpp
	src/TestCase.cpp
	src/TestSuite.hpp
	src/TestSuite.cpp
	src/Utils.hpp)

add_executable(cps3_test ${CPS3_SRCS})
target_link_libraries(cps3_test boost_system boost_program_options pthread)
