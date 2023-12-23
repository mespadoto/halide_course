cmake_minimum_required(VERSION 3.16)

project(example)

set(CMAKE_PREFIX_PATH /scratch/Halide-16.0.0-x86-64-linux/lib/cmake)
set(CMAKE_CXX_STANDARD 17)

cmake_host_system_information(RESULT _host_name QUERY HOSTNAME)

if (_host_name MATCHES "m1")
    set(OpenMP_CXX_FLAGS "-fopenmp-simd")
else()
    set(OpenMP_CXX_FLAGS "-fopenmp=libomp")
endif()

set(OpenMP_CXX_LIB_NAMES "libomp")
set(OpenMP_libomp_LIBRARY "/usr/lib/llvm-14/lib/libomp.so")

include_directories(/scratch/Halide-16.0.0-x86-64-linux/share/Halide/tools)

find_package(OpenMP REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Halide REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE Halide::Halide ${PNG_LIBRARY} ${JPEG_LIBRARY} OpenMP::OpenMP_CXX)
