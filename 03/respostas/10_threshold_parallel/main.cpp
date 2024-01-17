#include "Halide.h"
#include "halide_image_io.h"
#include <iostream>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using namespace Halide::Tools;

duration<double, std::milli> run_func(Halide::Buffer<uint8_t> &input, Halide::Buffer<uint8_t>& output, Halide::Func& func){
    auto t1 = high_resolution_clock::now();

    output = func.realize({input.width(), input.height(), input.channels()});

    auto t2 = high_resolution_clock::now();

    return t2 - t1;
}

Halide::Func get_basic_func(Halide::Buffer<uint8_t>& input, float threshold) {
  Halide::Func proc;
  Halide::Var x, y, c;
  Halide::Func float_px;
  
  float_px(x, y, c) = Halide::cast<float>(input(x, y, c) / 255.0f);
  Halide::Expr gray = 0.299f * float_px(x, y, 0) + 
                      0.587f * float_px(x, y, 1) + 
                      0.114f * float_px(x, y, 2);
  
  Halide::Expr thresholded = Halide::cast<uint8_t>(gray > threshold) * 255;
  
  proc(x, y, c) = thresholded;

  return proc;
}

Halide::Func get_parallel_func(Halide::Buffer<uint8_t>& input, float threshold) {
  Halide::Func proc;
  Halide::Var x, y, c;
  Halide::Func float_px;
  
  float_px(x, y, c) = Halide::cast<float>(input(x, y, c) / 255.0f);
  Halide::Expr gray = 0.299f * float_px(x, y, 0) + 
                      0.587f * float_px(x, y, 1) + 
                      0.114f * float_px(x, y, 2);
  
  Halide::Expr thresholded = Halide::cast<uint8_t>(gray > threshold) * 255;
  
  proc(x, y, c) = thresholded;

  Halide::Var x_outer, y_outer, x_inner, y_inner, tile_index;
  proc.tile(x, y, x_outer, y_outer, x_inner, y_inner, 1024, 1024)
      .fuse(x_outer, y_outer, tile_index)
      .parallel(tile_index);

  Halide::Var x_inner_outer, y_inner_outer, x_vectors, y_pairs;

  proc.tile(x_inner, y_inner, x_inner_outer, y_inner_outer, x_vectors, y_pairs, 8, 4)
      .vectorize(x_vectors)
      .unroll(y_pairs);

  return proc;
}

int main(int argc, char **argv) {
  float threshold = 0.5f;

  if (argc == 2) {
    threshold = atof(argv[1]);
  }
  
  Halide::Buffer<uint8_t> input = Halide::Tools::load_image("../bird_large.jpg");

  Halide::Func proc_basic = get_basic_func(input, threshold);
  Halide::Func proc_parallel = get_parallel_func(input, threshold);

  try {
    proc_basic.compile_jit(Halide::get_jit_target_from_environment());
    proc_parallel.compile_jit(Halide::get_jit_target_from_environment());

    Halide::Buffer<uint8_t> output_basic(input.width(), input.height());
    Halide::Buffer<uint8_t> output_parallel(input.width(), input.height());

    duration<double, std::milli> duration_basic = run_func(input, output_basic, proc_basic);
    duration<double, std::milli> duration_parallel = run_func(input, output_parallel, proc_parallel);

    std::cout   << "basic: " << duration_basic.count()
                << " ms, parallel: " << duration_parallel.count() << " ms" << std::endl;

    Halide::Tools::save_image(output_basic, "bird_binary_basic.png");
    Halide::Tools::save_image(output_parallel, "bird_binary_parallel.png");
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
