#include "../03_aot_build/blur.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv) {
  int k_size = 7;

  if (argc == 2) {
    k_size = atoi(argv[1]);
  }

  int k_start = -(k_size / 2);
  int k_sq = k_size * k_size;

  Halide::Runtime::Buffer<uint8_t> input = Halide::Tools::load_image("../bird_large.jpg");
  Halide::Runtime::Buffer<uint8_t> output(input.width(), input.height(), input.channels());

  int error = my_halide_blur(input, k_size, k_start, k_sq, output);

  if (error) {
      std::cout << "Halide returned an error: " <<  error << std::endl;
      return 1;
  }

  Halide::Tools::save_image(output, "bird_blurred.png");

  return 0;
}
