#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x("x"), y("y");
  Halide::Func gradient("gradient");

  gradient(x, y) = x + y;

  Halide::Var x_outer, y_outer, x_inner, y_inner, tile_index;

  // particiona o problema em tiles de 64x64 para serem processados em paralelo
  gradient.tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64)
          .fuse(x_outer, y_outer, tile_index)
          .parallel(tile_index);

// para cada tile, processa duas linhas em paralelo com vetorização em x
// 
// para isso:
// - recursivamente divide cada tile de 64x64 em subtiles de 4x2
// - vetoriza os subtiles em x
// - e os desenrola em y
  Halide::Var x_inner_outer, y_inner_outer, x_vectors, y_pairs;
  
  gradient.tile(x_inner, y_inner, x_inner_outer, y_inner_outer, x_vectors, y_pairs, 4, 2)
          .vectorize(x_vectors)
          .unroll(y_pairs);

  // //equivalente em C++ serial
  // for (int tile_index = 0; tile_index < 6 * 4; tile_index++) {
  //   int y_outer = tile_index / 4;
  //   int x_outer = tile_index % 4;
  //   for (int y_inner_outer = 0; y_inner_outer < 64 / 2; y_inner_outer++) {
  //     for (int x_inner_outer = 0; x_inner_outer < 64 / 4; x_inner_outer++) {
  //       // We're vectorized across x
  //       int x = std::min(x_outer * 64, 350 - 64) + x_inner_outer * 4;
  //       int x_vec[4] = {x + 0,
  //                       x + 1,
  //                       x + 2,
  //                       x + 3};

  //       // And we unrolled across y
  //       int y_base = std::min(y_outer * 64, 250 - 64) + y_inner_outer * 2;
  //       {
  //           // y_pairs = 0
  //           int y = y_base + 0;
  //           int y_vec[4] = {y, y, y, y};
  //           int val[4] = {x_vec[0] + y_vec[0],
  //                         x_vec[1] + y_vec[1],
  //                         x_vec[2] + y_vec[2],
  //                         x_vec[3] + y_vec[3]};

  //           // Check the result.
  //           for (int i = 0; i < 4; i++) {
  //               if (result(x_vec[i], y_vec[i]) != val[i]) {
  //                   printf("There was an error at %d %d!\n",
  //                           x_vec[i], y_vec[i]);
  //                   return -1;
  //               }
  //           }
  //       }
  //       {
  //           // y_pairs = 1
  //           int y = y_base + 1;
  //           int y_vec[4] = {y, y, y, y};
  //           int val[4] = {x_vec[0] + y_vec[0],
  //                         x_vec[1] + y_vec[1],
  //                         x_vec[2] + y_vec[2],
  //                         x_vec[3] + y_vec[3]};

  //           // Check the result.
  //           for (int i = 0; i < 4; i++) {
  //               if (result(x_vec[i], y_vec[i]) != val[i]) {
  //                   printf("There was an error at %d %d!\n",
  //                           x_vec[i], y_vec[i]);
  //                   return -1;
  //               }
  //           }
  //       }
  //     }
  //   }
  // }

  gradient.trace_stores();
  gradient.print_loop_nest();

  try {
    Halide::Buffer<int32_t> output = gradient.realize({350, 250});
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
