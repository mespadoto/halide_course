#include <vector>
#include <thread>
#include <future>
#include <iostream>
#include <chrono>
#include <cstdlib>

#include "Halide.h"
#include "halide_image_io.h"

#include <immintrin.h>

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using namespace Halide::Tools;

duration<double, std::milli> naive_blur(const Halide::Buffer<uint16_t> &in, Halide::Buffer<uint16_t> &bv) {
    auto t1 = high_resolution_clock::now();

    for (int y = 1; y < in.height(); y++) {
        for (int x = 1; x < in.width(); x++) {
            uint32_t sum = 0;
            for (int dy = -1; dy < 2; dy++) {
                for (int dx = -1; dx < 2; dx++) {
                    sum += in(x + dx, y + dy);
                }
            }
            bv(x,y) = sum / 9;
        }
    }

    auto t2 = high_resolution_clock::now();
    return t2 - t1;
}

duration<double, std::milli> clever_blur(const Halide::Buffer<uint16_t> &in, Halide::Buffer<uint16_t> &bv) {
    auto t1 = high_resolution_clock::now();
    
    Halide::Buffer<uint16_t> bh(in.width(), in.height());

    for (int y = 1; y < in.height()-1; y++)
        for (int x = 1; x < in.width()-1; x++)
            bh(x, y) = (in(x-1, y) + in(x, y) + in(x+1, y))/3;

    for (int y = 1; y < in.height()-1; y++)
        for (int x = 1; x < in.width()-1; x++)
            bv(x, y) = (bh(x, y-1) + bh(x, y) + bh(x, y+1))/3;
    
    auto t2 = high_resolution_clock::now();
    return t2 - t1;
}

duration<double, std::milli> fast_blur(const Halide::Buffer<uint16_t> &in, Halide::Buffer<uint16_t> &bv) {    
    auto t1 = high_resolution_clock::now();

  __m128i one_third = _mm_set1_epi16(21846);
  #pragma omp parallel for
  for (int yTile = 0; yTile < in.height(); yTile += 32) {
    __m128i a, b, c, sum, avg;
    __m128i bh[(256/8)*(32+2)];
    for (int xTile = 0; xTile < in.width(); xTile += 256) {
      __m128i *bhPtr = bh;
      for (int y = -1; y < 32+1; y++) {
        const uint16_t *inPtr = &(in(xTile, yTile+y));
        for (int x = 0; x < 256; x += 8) {
          a = _mm_loadu_si128((__m128i*)(inPtr - 1));
          b = _mm_loadu_si128((__m128i*)(inPtr + 1));
          c = _mm_load_si128 ((__m128i*)(inPtr));
          sum = _mm_add_epi16(_mm_add_epi16(a, b), c);
          avg = _mm_mulhi_epi16(sum, one_third);
          _mm_store_si128(bhPtr++, avg);
          inPtr += 8;
        }
      }
      bhPtr = bh;
      for (int y = 0; y < 32; y++) {
        __m128i *outPtr = (__m128i *)(&(bv(xTile, yTile+y)));
        for (int x = 0; x < 256; x += 8) {
          a = _mm_load_si128(bhPtr + (256 * 2) / 8);
          b = _mm_load_si128(bhPtr + 256 / 8);
          c = _mm_load_si128(bhPtr++);
          sum = _mm_add_epi16(_mm_add_epi16(a, b), c);
          avg = _mm_mulhi_epi16(sum, one_third);
          _mm_store_si128(outPtr++, avg);
        }
      }
    }
  }
    auto t2 = high_resolution_clock::now();
    return t2 - t1;
}

duration<double, std::milli> halide_blur(const Halide::Buffer<uint16_t> &in, Halide::Buffer<uint16_t> &out) {
    Halide::Func bh, bv;
    Halide::Var x, y, xi, yi;


    Halide::Func clamped = Halide::BoundaryConditions::repeat_edge(in);
    bh(x, y) = (clamped(x-1, y) + clamped(x, y) + clamped(x+1, y))/3;
    bv(x, y) = (bh(x, y-1) + bh(x, y) + bh(x, y+1))/3;

    bv.tile(x, y, xi, yi, 256, 32).vectorize(xi, 8).parallel(y);
    bh.compute_at(bv, x).vectorize(x, 8);

    bh.compile_jit(Halide::get_jit_target_from_environment());

    auto t1 = high_resolution_clock::now();

    out = bh.realize({in.width(), in.height()});

    auto t2 = high_resolution_clock::now();
    return t2 - t1;
}

int main(int argc, char **argv) {
    Halide::Buffer<uint16_t> input = load_and_convert_image("gray.png");
    Halide::Buffer<uint16_t> output(input.width(), input.height());

    duration<double, std::milli> ms_naive = naive_blur(input, output);
    duration<double, std::milli> ms_clever = clever_blur(input, output);
    duration<double, std::milli> ms_fast = fast_blur(input, output);
    duration<double, std::milli> ms_halide = halide_blur(input, output);

    std::cout   << "naive: " << ms_naive.count()
                << " ms, clever: " << ms_clever.count()
                << " ms, fast: " << ms_fast.count()
                << " ms, halide: " << ms_halide.count() << " ms" << std::endl;

    return 0;
}
