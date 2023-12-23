#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x("x"), y("y");
  Halide::Func gradient("gradient");

  gradient(x, y) = x + y;
  gradient.vectorize(x, 4);

  // //equivalente a 
  // Halide::Var x_outer, x_inner;
  // gradient.split(x, x_outer, x_inner, 4);
  // gradient.vectorize(x_inner);

  // //equivalente em C++
  // for (int y = 0; y < 4; y++) {
  //   for (int x_outer = 0; x_outer < 2; x_outer++) {
  //     //na pratica esse código é implementado com instruções SSE
  //     int x_vec[] = {x_outer * 4 + 0,
  //                     x_outer * 4 + 1,
  //                     x_outer * 4 + 2,
  //                     x_outer * 4 + 3};
  //     int val[] = {x_vec[0] + y,
  //                   x_vec[1] + y,
  //                   x_vec[2] + y,
  //                   x_vec[3] + y};
  //   }
  // }

  gradient.trace_stores();
  gradient.print_loop_nest();

  try {
    Halide::Buffer<int32_t> output = gradient.realize({8, 4});
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
