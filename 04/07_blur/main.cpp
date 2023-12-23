#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Buffer<uint8_t> input = Halide::Tools::load_image("../bird_large.jpg");

  Halide::Var x, y, c;
  Halide::Func clamped, input_16, blur_x, blur_y, proc;

  // repete bordas
  clamped = Halide::BoundaryConditions::repeat_edge(input);

  // converte para uint16
  input_16(x, y, c) = Halide::cast<uint16_t>(clamped(x, y, c));

  // blur na horizontal
  blur_x(x, y, c) = ( input_16(x - 2, y, c) +
                      input_16(x - 1, y, c) +
                      input_16(x, y, c) +
                      input_16(x + 1, y, c) +
                      input_16(x + 2, y, c)) / 5;

  // blur na vertical
  blur_y(x, y, c) = ( blur_x(x, y - 2, c) +
                      blur_x(x, y - 1, c) +
                      blur_x(x, y, c) +
                      blur_x(x, y + 1, c) + 
                      blur_x(x, y + 2, c)) / 5;

  //converte para uint8  
  proc(x, y, c) = Halide::cast<uint8_t>(blur_y(x, y, c));

  try {
    Halide::Buffer<uint8_t> output = proc.realize({input.width(), input.height(), 3});
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
