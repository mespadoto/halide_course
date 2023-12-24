#include "Halide.h"
#include "halide_image_io.h"
#include <cstdlib>

int main(int argc, char **argv) {

  try{
    Halide::Var x, y, c;
    Halide::Func clamped("clamped");
    Halide::Func local_sum("local_sum");
    Halide::Func blur("blur");

    Halide::Param<int32_t> k_size("k_size");
    Halide::Param<int32_t> k_start("k_start");
    Halide::Param<int32_t> k_sq("k_sq");
    Halide::ImageParam input(Halide::type_of<uint8_t>(), 3, "input");

    Halide::RDom r(k_start, k_size, k_start, k_size);
    clamped = Halide::BoundaryConditions::repeat_edge(input);

    local_sum(x, y, c) = 0;  //força uso de int32
    local_sum(x, y, c) += clamped(x + r.x, y + r.y, c);

    blur(x, y, c) = Halide::cast<uint8_t>(local_sum(x, y, c) / k_sq);
    blur.compile_to_static_library("blur", {input, k_size, k_start, k_sq}, "my_halide_blur");
  }
  catch (Halide::CompileError& e){
    std::cout << "Halide::CompileError: " << e.what();
  }
  catch (Halide::RuntimeError& e){
    std::cout << "Halide::RuntimeError: " << e.what();
  }
  catch (std::exception& e){
    std::cout << "Generic Error: " << e.what();
  }

  return 0;
}
