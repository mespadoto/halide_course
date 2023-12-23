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

  producer.store_root();
  producer.compute_at(consumer, y);

  consumer.trace_stores();
  producer.trace_stores();
  consumer.print_loop_nest();

  // //equivalente em C++ 
  // float result[4][4];

  // // producer.store_root() implies that storage goes here:
  // float producer_storage[5][5];

  // // There's an outer loop over scanlines of consumer:
  // for (int y = 0; y < 4; y++) {

  //     // Compute enough of the producer to satisfy this scanline
  //     // of the consumer.
  //     for (int py = y; py < y + 2; py++) {

  //         // Skip over rows of producer that we've already
  //         // computed in a previous iteration.
  //         if (y > 0 && py == y) continue;

  //         for (int px = 0; px < 5; px++) {
  //             producer_storage[py][px] = sin(px * py);
  //         }
  //     }

  //     // Compute a scanline of the consumer.
  //     for (int x = 0; x < 4; x++) {
  //         result[y][x] = (producer_storage[y][x] +
  //                         producer_storage[y + 1][x] +
  //                         producer_storage[y][x + 1] +
  //                         producer_storage[y + 1][x + 1]) / 4;
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
