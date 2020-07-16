cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

add_definitions(-std=c++11)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(PointClouds_Kitti2pcd)

add_executable (kitti2pcd src/kitti2pcd.cpp)
target_link_libraries (kitti2pcd)

