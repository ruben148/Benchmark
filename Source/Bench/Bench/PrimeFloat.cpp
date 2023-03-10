#include "benchmark.hpp"

using namespace benchmark;

bool PrimeFloat::is_prime(double a)
{
	if (a < 2)
		return false;
	double dividend = 0;
	float divisors = 0;
	for (double j = 2; j <= sqrt(a); j++)
	{
		dividend = 1.0 * a / j;
		if ((dividend - fmod(dividend, 1)) * j == a)
			divisors += 1.0;
	}
	if (divisors > 0)
		return false;
	return true;
}

void PrimeFloat::thread_function(std::vector<double> q, int* number_of_primes, int* time_difference)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	begin = std::chrono::steady_clock::now();
	for (int a : q)
		if (is_prime(a))
			(*number_of_primes)++;
	end = std::chrono::steady_clock::now();
	(*time_difference) = std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count();
}

void PrimeFloat::run(std::vector<std::thread>& thread_pool)
{
	running = true;
	int number_of_threads = args[0];
	int first = args[1];
	int last = args[2];

	std::vector<std::vector<double>> numbers(number_of_threads);

	for (int i = 0; i < number_of_threads; i++)
		for (int j = first + i; j <= last; j += number_of_threads)
			numbers[i].push_back(j);

	results = (int*)calloc(number_of_threads * 2, sizeof(int));

	int real_thread_count = std::thread::hardware_concurrency();

	for (int i = 0; i < number_of_threads; i++)
	{
		thread_pool.push_back(std::thread(&PrimeFloat::thread_function, this, numbers[i], results + number_of_threads + i, results + i));
		if (number_of_threads == real_thread_count) {
			auto mask = (static_cast<DWORD_PTR>(1) << i);
			DWORD_PTR dw = SetThreadAffinityMask(thread_pool.back().native_handle(), mask);
		}
		else {
			auto mask = (static_cast<DWORD_PTR>(1) << i * 2);
			DWORD_PTR dw = SetThreadAffinityMask(thread_pool.back().native_handle(), mask);
		}
	}
}
