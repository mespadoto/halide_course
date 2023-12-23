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

  Halide::Var x_outer, y_outer, x_inner, y_inner;
  consumer.tile(x, y, x_outer, y_outer, x_inner, y_inner, 4, 4);

  // computa producer para cada tile do consumer
  producer.compute_at(consumer, x_outer);

  consumer.trace_stores();
  producer.trace_stores();
  consumer.print_loop_nest();

  try {
    consumer.realize({8, 8});
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
