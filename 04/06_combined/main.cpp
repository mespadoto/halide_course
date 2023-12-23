#include "Halide.h"
#include "halide_image_io.h"

int main(int argc, char **argv) {
  Halide::Var x, y;
  Halide::Func producer, consumer;

  producer(x, y) = sin(x * y);

  consumer(x, y) = (producer(x, y) +
                    producer(x, y + 1) +
                    producer(x + 1, y) +
                    producer(x + 1, y + 1)) / 4;

  Halide::Var yo, yi;

  // Split the y coordinate of the consumer into strips of 16 scanlines:
  consumer.split(y, yo, yi, 16);
  // Compute the strips using a thread pool and a task queue.
  consumer.parallel(yo);
  // Vectorize across x by a factor of four.
  consumer.vectorize(x, 4);

  // Now store the producer per-strip. This will be 17 scanlines
  // of the producer (16+1), but hopefully it will fold down
  // into a circular buffer of two scanlines:
  producer.store_at(consumer, yo);
  // Within each strip, compute the producer per scanline of the
  // consumer, skipping work done on previous scanlines.
  producer.compute_at(consumer, yi);
  // Also vectorize the producer (because sin is vectorizable on x86 using SSE).
  producer.vectorize(x, 4);

  try {
    consumer.realize({160, 160});
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
