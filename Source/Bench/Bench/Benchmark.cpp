#include "benchmark.hpp"
#include "CPU_Specs.h"

using namespace benchmark;

void BenchmarkAlgorithm::run_for(int number_of_threads, int seconds)
{
	message_queue bench_to_gui(open_only, "BENCH_TO_GUI");
	//int availableMemory = cpuSpecs.getAvailableMemory();
	//int usage = cpuSpecs.getUsage();
	//int frequency = cpuSpecs.getFrequency();
	//bench_to_gui.send(&usage, sizeof(int), 0);
	//bench_to_gui.send(&frequency, sizeof(int), 0);
	//bench_to_gui.send(&availableMemory, sizeof(int), 0);

	FixedQueue<int, 5> last_scores;
	int tries = 1;
	initialize_args(number_of_threads);

	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	begin = std::chrono::steady_clock::now();

	number_of_increments = 1;

	for (int i = 1, total_duration = 0, score = 0;
		total_duration < seconds * 1000000 || args[upper_limit_index] > max_upper_limit;
		i++, total_duration = std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count())
	{
		int duration = 0;
		for (int i = 0; i < tries; i++)
		{
			std::vector<std::thread> thread_pool;
			run_multithreaded(thread_pool, true);
			duration = 0;
			for (int j = 0; j < number_of_threads; j++)
				duration += results[j];
		}
		duration = duration / number_of_threads / tries;
		score = get_score(duration, args[upper_limit_index], number_of_increments);

		last_scores.push(score);
		int sc = last_scores.average();
		std::cout << "Sending score " << last_scores.average() << "...";
		bench_to_gui.send(&sc, sizeof(int), 0);
		std::cout << "Done sending." << std::endl;
		if (last_scores.full) {

		}

		if (duration < 500000 && number_of_increments < 5)
			number_of_increments++;
		if (duration > 700000 && number_of_increments > 1)
			number_of_increments--;

		if (increment_lower_limit)
			args[lower_limit_index] = args[upper_limit_index];
		if (increment_upper_limit)
			args[upper_limit_index] += increment * number_of_increments;

		end = std::chrono::steady_clock::now();
	}
	int msg = -2;
	bench_to_gui.send(&msg, sizeof(int), 0);
	bench_to_gui.send(&msg, sizeof(int), 0);
	bench_to_gui.send(&msg, sizeof(int), 0);
	bench_to_gui.send(&msg, sizeof(int), 0);

}

using namespace boost::interprocess;

int __main() {

	CPU_Specs cpuSpecs;
	cpuSpecs.retrieveInformation();


	message_queue bench_to_gui(open_only, "BENCH_TO_GUI");

	message_queue gui_to_bench(open_only, "GUI_TO_BENCH");

	while(true)
		try{
			unsigned int priority;
			message_queue::size_type recvd_size;

			int number;
			gui_to_bench.receive(&number, sizeof(int), recvd_size, priority);

			switch (number){
			case 0:
				try {
					for (int i = 0; i < 64; i++)
						bench_to_gui.send(&cpuSpecs.brand[i], sizeof(char), 0);
					for (int i = 0; i < 32; i++)
						bench_to_gui.send(&cpuSpecs.vendor[i], sizeof(char), 0);
				}
				catch (interprocess_exception& ex)
				{
					
				}
				break;
			}
		}
		catch (interprocess_exception& ex) {
			
		}


	return 0;
}