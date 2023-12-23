cmake_minimum_required(VERSION 3.16)

SET (CMAKE_CXX_COMPILER "/usr/bin/clang++-15" CACHE STRING "C++ compiler" FORCE)

project(blur)

set(CMAKE_PREFIX_PATH /tmp/Halide-16.0.0-x86-64-linux/lib/cmake)
set(CMAKE_CXX_STANDARD 17)

set(OpenMP_CXX_FLAGS "-fopenmp=libomp")
set(OpenMP_CXX_LIB_NAMES "libomp")
set(OpenMP_libomp_LIBRARY "/usr/lib/llvm-14/lib/libomp.so")

include_directories(/tmp/Halide-16.0.0-x86-64-linux/share/Halide/tools)

find_package(OpenMP REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Halide REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE Halide::Halide ${PNG_LIBRARY} ${JPEG_LIBRARY} OpenMP::OpenMP_CXX)
