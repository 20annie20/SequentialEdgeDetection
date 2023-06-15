cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project (SequentialEdgeDetection)

# Add source to this project's executable.
add_executable (SequentialEdgeDetection SequentialEdgeDetection.cpp SequentialEdgeDetection.h)
target_include_directories(SequentialEdgeDetection PRIVATE ${CMAKE_SOURCE_DIR}/external/stb/stb_image)