#include "Halide.h"
#include "halide_image_io.h"
#include <cstdlib>

using namespace Halide::Tools;

int main(int argc, char **argv) {
  float threshold = 0.5f;

  if (argc == 2) {
    threshold = atof(argv[1]);
  }
  
  Halide::Buffer<uint8_t> input = Halide::Tools::load_image("bird.jpg");

  Halide::Func proc;
  Halide::Var x, y, c;
  Halide::Func float_px;
  
  float_px(x, y, c) = Halide::cast<float>(input(x, y, c) / 255.0f);
  Halide::Expr gray = 0.299f * float_px(x, y, 0) + 
                      0.587f * float_px(x, y, 1) + 
                      0.114f * float_px(x, y, 2);
  
  Halide::Expr thresholded = Halide::cast<uint8_t>(gray > threshold) * 255;
  
  proc(x, y, c) = thresholded;

  try {
    Halide::Buffer<uint8_t> output = proc.realize(
      {input.width(), input.height(), input.channels()});

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


//   //Halide::Expr uint_px = print(Halide::cast<uint8_t>(convert_gray * 255), "x: ", x, "y: ", y, " = ", convert_gray);
//   //Halide::Expr uint_px = print_when(convert_gray == 0.0f, Halide::cast<uint8_t>(convert_gray * 255), "x: ", x, "y: ", y, " = ", convert_gray);

//   // proc.trace_loads();
//   // proc.trace_realizations();
//   //proc.trace_stores();

// //  proc.compile_to_lowered_stmt("proc.stmt.html", {}, Halide::HTML);
