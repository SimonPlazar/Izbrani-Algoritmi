cmake_minimum_required(VERSION 3.26)
project(IA_N3)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/boost_1_66_0)

find_package(Boost 1.66.0 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})


add_executable(IA_N3 main.cpp)

link_directories(${BOOST_ROOT}/lib)

target_link_libraries(IA_N3 ${Boost_LIBRARIES})
