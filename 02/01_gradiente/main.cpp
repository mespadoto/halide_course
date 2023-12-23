#include "Halide.h"
#include "halide_image_io.h"

using namespace Halide::Tools;

int main(int argc, char **argv) {
  Halide::Func gradient;
  Halide::Var x, y;
  Halide::Expr e = Halide::cast<uint8_t>(((x + y) / 1600.0f) * 255);
  gradient(x, y) = e;

  try {
    Halide::Buffer<uint8_t> output = gradient.realize({800, 800});
    save_image(output, "gradient.png");
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



