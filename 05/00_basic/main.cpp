#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x, y;
  Halide::Func g;
  g(x, y) = x + y;
  g(2, 1) = 42;
  g(x, 0) = g(x, 1);

  g.trace_loads();
  g.trace_stores();

  try {
    g.realize({4, 4});
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
