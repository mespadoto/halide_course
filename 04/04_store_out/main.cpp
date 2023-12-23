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
  producer.compute_at(consumer, x);

  consumer.trace_stores();
  producer.trace_stores();
  consumer.print_loop_nest();

  // //equivalente em C++ 
  // float result[4][4];

  // // producer.store_root() implies that storage goes here, but
  // // we can fold it down into a circular buffer of two
  // // scanlines:
  // float producer_storage[2][5];

  // // For every pixel of the consumer:
  // for (int y = 0; y < 4; y++) {
  //     for (int x = 0; x < 4; x++) {

  //         // Compute enough of the producer to satisfy this
  //         // pixel of the consumer, but skip values that we've
  //         // already computed:
  //         if (y == 0 && x == 0) {
  //             producer_storage[y & 1][x] = sin(x * y);
  //         }
  //         if (y == 0) {
  //             producer_storage[y & 1][x + 1] = sin((x + 1) * y);
  //         }
  //         if (x == 0) {
  //             producer_storage[(y + 1) & 1][x] = sin(x * (y + 1));
  //         }
  //         producer_storage[(y + 1) & 1][x + 1] = sin((x + 1) * (y + 1));

  //         result[y][x] = (producer_storage[y & 1][x] +
  //                         producer_storage[(y + 1) & 1][x] +
  //                         producer_storage[y & 1][x + 1] +
  //                         producer_storage[(y + 1) & 1][x + 1]) / 4;
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
