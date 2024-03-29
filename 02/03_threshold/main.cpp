#include "Halide.h"
#include "halide_image_io.h"
#include <cstdlib>

using namespace Halide::Tools;

int main(int argc, char **argv) {
  float threshold = 0.5f;
  
  Halide::Buffer<uint8_t> input = Halide::Tools::load_image("bird.jpg");

  Halide::Func proc;
  Halide::Var x, y, c;

  //TODO: calcular o threshold
  //proc(x, y, c) = xxxxxx;

  try {
    Halide::Buffer<uint8_t> output = proc.realize({input.width(), input.height(), input.channels()});
    Halide::Tools::save_image(output, "bird_binary.png");
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
