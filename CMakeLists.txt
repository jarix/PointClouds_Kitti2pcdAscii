cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(kitti2pcd)

add_definitions(-std=c++11)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

find_package(Boost 1.60.0 COMPONENTS filesystem program_options)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable (kitti2pcd src/kitti2pcd.cpp)
target_link_libraries (kitti2pcd ${Boost_LIBRARIES})

