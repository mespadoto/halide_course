#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x, y;
  Halide::Func f("f");
  
  //função original
  f(x, y) = (x + y) / 100.0f;

  //queremos elevar as 2 primeiras linhas ao quadrado
  //opção 3: domínio de redução:
  Halide::RDom r(0, 2);
  f(x, r) = f(x, r) * f(x, r);

  //opção 2: gerar código com loop em C++
  for (int i = 0; i < 2; i++) {
    f(x, i) = f(x, i) * f(x, i);
  }

  //opção 1: escrever na mão
  f(x, 0) = f(x, 0) * f(x, 0);
  f(x, 1) = f(x, 1) * f(x, 1);

  f.trace_stores();
  f.print_loop_nest();

  try {
    Halide::Buffer<float> halide_result = f.realize({10, 10});
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
