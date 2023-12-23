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

  producer.compute_at(consumer, y);

  consumer.trace_stores();
  producer.trace_stores();
  consumer.print_loop_nest();

  // //equivalente em C++ 
  // float result[4][4];

  // // loop externo sobre as linhas do consumer
  // for (int y = 0; y < 4; y++) {

  //     // calcula producer para a linha do consumer
  //     float producer_storage[2][5];
  //     for (int py = y; py < y + 2; py++) {
  //         for (int px = 0; px < 5; px++) {
  //             producer_storage[py - y][px] = sin(px * py);
  //         }
  //     }

  //     // consumer
  //     for (int x = 0; x < 4; x++) {
  //         result[y][x] = (producer_storage[0][x] +
  //                         producer_storage[1][x] +
  //                         producer_storage[0][x + 1] +
  //                         producer_storage[1][x + 1]) / 4;
  //     }
  // }

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
