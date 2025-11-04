cmake_minimum_required(VERSION 3.16)

project(students_db)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)

add_library(v1 src/v11.cpp src/v12.cpp src/v13.cpp src/v14.cpp)
target_include_directories(v1 PUBLIC include)
add_sanitizers(v1)



add_executable(students_db src/main.cpp)
target_include_directories(students_db PUBLIC include)
target_link_libraries(students_db PUBLIC v1)
# target_link_libraries(students_db PUBLIC s4)
add_sanitizers(students_db)
target_compile_options(students_db PRIVATE -O2 -g)
