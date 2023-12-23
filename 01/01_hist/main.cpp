#include <vector>
#include <thread>
#include <future>
#include <iostream>
#include <chrono>
#include <cstdlib>

#include "Halide.h"
#include "halide_image_io.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration;
using namespace Halide::Tools;

std::vector<int> compute_hist_per_tile(const Halide::Buffer<uint8_t> &input, int start_x, int start_y, int end_x, int end_y)
{
	std::vector<int> result(256, 0);

	for (int r_y = start_y; r_y < end_y; r_y++) 
	{
	    for (int r_x = start_x; r_x < end_x; r_x++) 
	    {
	        result[input(r_x, r_y)] += 1;
	    }
	}

	return result;
}

duration<double, std::milli> run_naive(Halide::Buffer<uint8_t> &input, int n_times)
{
    auto t1 = high_resolution_clock::now();

    for (int i = 0; i < n_times; i++) 
    {
        std::vector<int> results;
        results = compute_hist_per_tile(input, 0, 0, input.width(), input.height()); 
    }

    auto t2 = high_resolution_clock::now();

    return t2 - t1;
}

duration<double, std::milli> run_tiled(Halide::Buffer<uint8_t> &input, int n_times)
{
    int n_tiles_per_dim = 2; //4 tiles
    int n_threads = n_tiles_per_dim * n_tiles_per_dim;
    
	int start_xs[n_threads];
	int start_ys[n_threads];
	int end_xs[n_threads];
	int end_ys[n_threads];

	int tile_height = input.height() / n_tiles_per_dim;
	int tile_width = input.width() / n_tiles_per_dim;

	for (int i  = 0; i < n_threads; i++)
	{
        int row = i / n_tiles_per_dim;
        int col = i % n_tiles_per_dim;
		
        start_ys[i] = row*tile_height;
		end_ys[i] = (row+1)*tile_height;

		start_xs[i] = col*tile_width;
		end_xs[i] = (col+1)*tile_width;
	}

	end_ys[n_threads-1] += input.height() - end_ys[n_threads-1];
	end_xs[n_threads-1] += input.width() - end_xs[n_threads-1];

	// for (int i  = 0; i < n_threads; i++)
	// {
	// 	std::cout 
    //         << "start_ys[" << i << "]: " << start_ys[i] 
    //         << ", end_ys[" << i << "]: "  << end_ys[i] 
    //         << ", start_xs[" << i << "]: " << start_xs[i] 
    //         << ", end_xs[" << i << "]: " << end_xs[i] << std::endl;
    // }

    auto t1 = high_resolution_clock::now();

    for (int i = 0; i < n_times; i++)
    {
        std::vector<int> result_tiled(256, 0);
        std::future<std::vector<int>> partial_results[n_threads];

        for (int i  = 0; i < n_threads; i++)
        {
            partial_results[i] = std::async(compute_hist_per_tile, input, start_xs[i], start_ys[i], end_xs[i], end_ys[i]);
        }

        for (int i  = 0; i < n_threads; i++)
        {
            std::vector<int> partial_result = partial_results[i].get();

            for (int j = 0; j < 256; j++)
            {
                result_tiled[j] += partial_result[j];
            }
        }
    }

    auto t2 = high_resolution_clock::now();

    return t2 - t1;
}

duration<double, std::milli> run_halide(Halide::Buffer<uint8_t> &input, int n_times, bool discard_setup_time)
{
    Halide::Func histogram("histogram");
    Halide::Var x("x");
    histogram(x) = 0;
    Halide::RDom r(0, input.width(), 0, input.height());
    histogram(input(r.x, r.y)) += 1;

    //compilação AOT
    if (discard_setup_time)
        histogram.compile_jit(Halide::get_jit_target_from_environment());

    auto t1 = high_resolution_clock::now();

    for(int i = 0; i < n_times; i++)
    {
        Halide::Buffer<int> halide_result = histogram.realize({256});
    }

    auto t2 = high_resolution_clock::now();

    return t2 - t1;
}

int main(int argc, char **argv) 
{
    bool discard_setup_time = false;
    int n_times = 1;

    if (argc >= 2)
        n_times = atoi(argv[1]);

    if (argc > 2)
        discard_setup_time = true;

    std::cout << "n_times: " << n_times << ", discard_setup_time: " << discard_setup_time << std::endl;

    Halide::Buffer<uint8_t> input = load_image("gray.png");

    //versao C++ naive, serial
    duration<double, std::milli> ms_naive = run_naive(input, n_times);

    //versao tiled, 4 threads
    duration<double, std::milli> ms_tiled = run_tiled(input, n_times);

    //versao Halide
    duration<double, std::milli> ms_halide = run_halide(input, n_times, discard_setup_time);

    std::cout   << "naive: " << ms_naive.count()/n_times
                << " ms, tiled: " << ms_tiled.count()/n_times 
                << " ms, halide: " << ms_halide.count()/n_times << " ms" << std::endl;

    // //checa resultados
    // for (int x = 0; x < 256; x++) 
    // {
    //     if (!(  result_naive[x] == result_tiled[x] 
    //             && result_tiled[x] == halide_result(x) 
    //             && halide_result(x) == result_naive[x]))
    //     {
    //         std::cout << "Divergence: " << x 
    //             << ": result_naive=" << result_naive[x] 
    //             << ", result_tiled=" << result_tiled[x] 
    //             << ", halide_result=" << halide_result(x) 
    //             << std::endl;
    //     }
    // }

    return 0;
}