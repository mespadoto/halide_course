#include "Halide.h"
#include "halide_image_io.h"
#include <cstdlib>

using namespace Halide::Tools;

int main(int argc, char **argv) {
  Halide::Buffer<uint8_t> input = Halide::Tools::load_image("bird.jpg");

  Halide::Func proc, float_px;
  Halide::Var x, y, c;

  //TODO: implementar
  //proc(x, y, c) = xxx;

  try {
    Halide::Buffer<uint8_t> output = proc.realize({input.width(), input.height(), input.channels()});
    Halide::Tools::save_image(output, "bird_sharper.png");
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
