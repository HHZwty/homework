cmake_minimum_required (VERSION 2.8)

project(homework)

add_executable (run main.cpp)

find_package(OpenCV REQUIRED)

target_link_libraries(run PRIVATE ${OpenCV_LIBS})