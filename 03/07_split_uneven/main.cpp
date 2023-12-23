#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x("x"), y("y");
  Halide::Func gradient("gradient");

  gradient(x, y) = x + y;

  Halide::Var x_outer, x_inner;
  gradient.split(x, x_outer, x_inner, 3);

  // //equivalente em C++
  // for (int y = 0; y < 2; y++) {
  //   for (int x_outer = 0; x_outer < 3; x_outer++) {
  //     for (int x_inner = 0; x_inner < 3; x_inner++) {
  //         int x = x_outer * 3;
  //         // checa limite
  //         if (x > 4) x = 4;
  //         x += x_inner;
  //         out = x + y;
  //     }
  //   }
  // }

  gradient.trace_stores();
  gradient.print_loop_nest();

  try {
    Halide::Buffer<int32_t> output = gradient.realize({7, 2});
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
