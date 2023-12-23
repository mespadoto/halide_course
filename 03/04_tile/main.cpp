#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x("x"), y("y");
  Halide::Func gradient("gradient");

  gradient(x, y) = x + y;

  Halide::Var x_outer, x_inner, y_outer, y_inner;
  gradient.tile(x, y, x_outer, y_outer, x_inner, y_inner, 4, 4);

  // // equivalente a
  // gradient.split(x, x_outer, x_inner, 4);
  // gradient.split(y, y_outer, y_inner, 4);
  // gradient.reorder(x_inner, y_inner, x_outer, y_outer);

  // //equivalente em C++
  // for (int y_outer = 0; y_outer < 2; y_outer++) {
  //   for (int x_outer = 0; x_outer < 2; x_outer++) {
  //     for (int y_inner = 0; y_inner < 4; y_inner++) {
  //       for (int x_inner = 0; x_inner < 4; x_inner++) {
  //           int x = x_outer * 4 + x_inner;
  //           int y = y_outer * 4 + y_inner;
  //           out = x + y;
  //       }
  //     }
  //   }
  // }

  gradient.trace_stores();
  gradient.print_loop_nest();

  try {
    Halide::Buffer<int32_t> output = gradient.realize({4, 4});
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
