#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x("x"), y("y");
  Halide::Func gradient("gradient");

  gradient(x, y) = x + y;

  Halide::Var x_outer, y_outer, x_inner, y_inner, tile_index;
  gradient.tile(x, y, x_outer, y_outer, x_inner, y_inner, 4, 4)
          .fuse(x_outer, y_outer, tile_index)
          .parallel(tile_index);

  // // equivalente em C++ serial
  // // o paralelismo deveria estar no loop externo
  // for (int tile_index = 0; tile_index < 4; tile_index++) {
  //   int y_outer = tile_index / 2;
  //   int x_outer = tile_index % 2;
  //   for (int y_inner = 0; y_inner < 4; y_inner++) {
  //     for (int x_inner = 0; x_inner < 4; x_inner++) {
  //         int y = y_outer * 4 + y_inner;
  //         int x = x_outer * 4 + x_inner;
  //         out = x + y;
  //     }
  //   }
  // }

  gradient.trace_stores();
  gradient.print_loop_nest();

  try {
    Halide::Buffer<int32_t> output = gradient.realize({8, 8});
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
