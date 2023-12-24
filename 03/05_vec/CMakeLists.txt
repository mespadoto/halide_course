cmake_minimum_required(VERSION 3.16)

project(example)

set(CMAKE_PREFIX_PATH /scratch/Halide-16.0.0-x86-64-linux/lib/cmake)
set(CMAKE_CXX_STANDARD 17)

include_directories(/scratch/Halide-16.0.0-x86-64-linux/share/Halide/tools)

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Halide REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE Halide::Halide ${PNG_LIBRARY} ${JPEG_LIBRARY})
