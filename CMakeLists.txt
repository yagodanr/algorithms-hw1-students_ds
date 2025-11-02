cmake_minimum_required(VERSION 3.16)

project(students_db)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(students_db src/main.cpp)
target_include_directories(students_db include/)