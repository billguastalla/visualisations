cmake_minimum_required(VERSION 2.8.12)

project(visualisations CXX)

get_filename_component(VIS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Visualisations ABSOLUTE)
get_filename_component(VIS_LIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Includes ABSOLUTE)
get_filename_component(VIS_LIB_BINARY_DIR ${CMAKE_BINARY_DIR}/lib ABSOLUTE)

find_package(boost)

