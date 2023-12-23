#include "Halide.h"
#include "halide_image_io.h"
#include <cstdlib>

int main(int argc, char **argv) {
  int k_size = 7;

  if (argc == 2) {
    k_size = atoi(argv[1]);
  }

  Halide::Buffer<uint8_t> input = Halide::Tools::load_image("../bird_large.jpg");

  Halide::Var x, y, c;
  Halide::Func clamped("clamped");
  Halide::Func local_sum("local_sum");
  Halide::Func blur("blur");

  int k_start = -(k_size / 2);
  int k_sq = k_size * k_size;

  Halide::RDom r(k_start, k_size, k_start, k_size);

  try {
    clamped = Halide::BoundaryConditions::repeat_edge(input);

    local_sum(x, y, c) = 0;  //força uso de int32
    local_sum(x, y, c) += clamped(x + r.x, y + r.y, c);

    blur(x, y, c) = Halide::cast<uint8_t>(local_sum(x, y, c) / k_sq);

    Halide::Buffer<uint8_t> output = blur.realize({input.width(), input.height(), input.channels()});
    Halide::Tools::save_image(output, "bird_blurred.png");
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
