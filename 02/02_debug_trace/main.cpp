#include "Halide.h"
#include "halide_image_io.h"

using namespace Halide::Tools;

int main(int argc, char **argv) {
  Halide::Buffer<uint8_t> input = Halide::Tools::load_image("bird.jpg");

  Halide::Func proc, func_px;
  Halide::Var x, y, c;
  Halide::Func float_px;
  
  float_px(x, y, c) = Halide::cast<float>(input(x, y, c) / 255.0f);
  Halide::Expr convert_gray = 0.299f * float_px(x, y, 0) + 
                              0.587f * float_px(x, y, 1) + 
                              0.114f * float_px(x, y, 2);
  
  Halide::Expr uint_px = Halide::cast<uint8_t>(convert_gray * 255);
  // Halide::Expr uint_px = print_when(convert_gray == 0.0f, 
  //                             Halide::cast<uint8_t>(convert_gray * 255),
  //                             "x: ", x, "y: ", y, " = ", convert_gray);

  //Halide::Expr uint_px = print(Halide::cast<uint8_t>(convert_gray * 255), "x: ", x, "y: ", y, " = ", convert_gray);

  proc(x, y, c) = uint_px;

  try {
    proc.compile_to_lowered_stmt("proc.stmt.html", {}, Halide::HTML);

    proc.print_loop_nest();
    
    Halide::Buffer<uint8_t> output = proc.realize(
      {input.width(), input.height(), input.channels()});

    Halide::Tools::save_image(output, "bird_gray.png");
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


  //Halide::Expr uint_px = print(Halide::cast<uint8_t>(convert_gray * 255), "x: ", x, "y: ", y, " = ", convert_gray);
  //Halide::Expr uint_px = print_when(convert_gray == 0.0f, Halide::cast<uint8_t>(convert_gray * 255), "x: ", x, "y: ", y, " = ", convert_gray);

  // proc.trace_loads();
  // proc.trace_realizations();
  //proc.trace_stores();

//  proc.compile_to_lowered_stmt("proc.stmt.html", {}, Halide::HTML);
