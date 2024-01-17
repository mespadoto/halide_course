#include "Halide.h"
#include "halide_image_io.h"
#include <iostream>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using namespace Halide::Tools;

int main(int argc, char **argv) {
  Halide::Buffer<uint8_t> input = Halide::Tools::load_image("../bird_large.jpg");

  //---------------------------------------------------------------
  // Definicao do algoritmo
  Halide::Func proc;
  Halide::Var x, y, c;
  Halide::Func float_px;
  
  float_px(x, y, c) = Halide::cast<float>(input(x, y, c) / 255.0f);
  Halide::Expr gray = 0.299f * float_px(x, y, 0) + 
                      0.587f * float_px(x, y, 1) + 
                      0.114f * float_px(x, y, 2);

    
  proc(x, y, c) = Halide::cast<uint8_t>(gray > threshold) * 255;
  //---------------------------------------------------------------

  //---------------------------------------------------------------
  // Definicao do scheduling. Por default, vai executar serialmente
  // TODO

  try {
    //compila o codigo Halide
    proc.compile_jit(Halide::get_jit_target_from_environment());

    //marca tempo de inicio
    auto t1 = high_resolution_clock::now();

    Halide::Buffer<uint8_t> output = proc.realize({input.width(), input.height(), input.channels()});

    //marca tempo de fim
    auto t2 = high_resolution_clock::now();
    //calcula a duracao
    duration<double, std::milli> duration = t2 - t1;

    std::cout << "time (ms): " << duration.count() << std::endl;

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
