#include "benchmark.hpp"
//#include "libxl.h"
#include <cstdio>

using namespace benchmark;

void BenchmarkAlgorithm::run_multithreaded(std::vector<std::thread> &thread_pool, bool join)
{
	int number_of_threads = args[0];
	thread_pool.reserve(number_of_threads);

	CPU_Specs cpuSpecs;
	cpuSpecs.init();

	run(thread_pool);

	try {
		message_queue bench_to_gui(open_only, "BENCH_TO_GUI");
		int availableMemory = cpuSpecs.getAvailableMemory();
		int usage = cpuSpecs.getUsage();
		int frequency = cpuSpecs.getFrequency();
		std::cout << "Sending usage, freq and availMemory...";
		bench_to_gui.send(&usage, sizeof(int), 0);
		bench_to_gui.send(&frequency, sizeof(int), 0);
		bench_to_gui.send(&availableMemory, sizeof(int), 0);
		std::cout << "Done sending." << std::endl;
	}
	catch (interprocess_exception& ex) {
		std::cout << ex.what() << std::endl;
	}

	if (join)
		for (int i = 0; i < number_of_threads; i++)
			thread_pool[i].join();
}

void BenchmarkAlgorithm::initialize_args(int number_of_threads)
{
	args.reserve(4);
	args[0] = number_of_threads;
	if (lower_limit)
		args[lower_limit_index] = 0;
	args[upper_limit_index] = increment;
}

int BenchmarkAlgorithm::get_score(int duration, int xValue, int number_of_increments)
{
	long double nominal_duration = 0;
	for (int j = 0; j < number_of_increments; j++)
	{
		for (int i = 0; i < complexity_polynomial.size(); i++)
			nominal_duration += complexity_polynomial[i] * pow(xValue, complexity_polynomial.size() - 1 - i);
		xValue -= increment;
	}
	if(!increment_upper_limit)
		return (((nominal_duration / duration) / number_of_increments) * 1000);
	return ((nominal_duration / duration) * 1000);
}

double BenchmarkAlgorithm::get_xValue(int duration, double min, double max)
{
	double xValue = (min + max) / 2;
	double nominal_duration = 0;
	for (int i = 0; i < complexity_polynomial.size(); i++)
		nominal_duration += complexity_polynomial[i] * pow(xValue, complexity_polynomial.size() - 1 - i);
	if (nominal_duration / duration >= 0.999 && nominal_duration / duration < 1.001)
		return xValue;
	else
		if (duration < nominal_duration)
			return get_xValue(duration, min, xValue);
		else
			return get_xValue(duration, xValue, max);
}