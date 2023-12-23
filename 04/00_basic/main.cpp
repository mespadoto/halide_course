#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x, y;
  Halide::Func producer("producer"), consumer("consumer");

  producer(x, y) = sin(x * y);

  consumer(x, y) = (producer(x, y) +
                    producer(x, y + 1) +
                    producer(x + 1, y) +
                    producer(x + 1, y + 1)) / 4;

  consumer.trace_stores();
  producer.trace_stores();
  consumer.print_loop_nest();

  // //equivalente em C++ 
  float result[4][4];
  for (int y = 0; y < 4; y++) {
      for (int x = 0; x < 4; x++) {
          result[y][x] = (sin(x * y) +
                          sin(x * (y + 1)) +
                          sin((x + 1) * y) +
                          sin((x + 1) * (y + 1))) / 4;
      }
  }

  try {
    consumer.realize({4, 4});
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
