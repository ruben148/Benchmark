#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <boost/interprocess/ipc/message_queue.hpp>
#include "Specs.hpp"
#include "Benchmark.hpp"
#include "MemTest.hpp"

using namespace boost::interprocess;
using namespace benchmark;

int main() {

	Specs specs;
	specs.retrieveInformation();
	specs.retrieveInstructions();
	specs.getDrives();
	//std::cout << "Instructions: " << specs.instructions << std::endl;
	//specs.PrintVolumes();

	//std::cout << specs.storageDevices << std::endl;

	while (true)
		try {
		message_queue gui_to_bench(open_only, "GUI_TO_BENCH");
		unsigned int priority;
		message_queue::size_type recvd_size;
		int number;
		gui_to_bench.receive(&number, sizeof(int), recvd_size, priority);
		int threads = 1;
		int seconds = 15;
		int allocation_size = 100000;
		switch (number) {
		case 9: /////////////////////////////////////////////////////SEND ALL INFO
		{
			try {
				message_queue bench_to_gui(open_only, "BENCH_TO_GUI");
				for (int i = 0; i < 64; i++) {
					bench_to_gui.send(&specs.brand[i], sizeof(char), 0);
				}
				for (int i = 0; i < 32; i++) {
					bench_to_gui.send(&specs.vendor[i], sizeof(char), 0);
				}
				for (int i = 0; i < 256; i++) {
					bench_to_gui.send(&specs.gpuName[i], sizeof(char), 0);
				}
				for (int i = 0; i < 1024; i++) {
					bench_to_gui.send(&specs.instructions[i], sizeof(char), 0);
				}
				for (int i = 0; i < 1024; i++) {
					bench_to_gui.send(&specs.drives[i], sizeof(char), 0);
				}
				for (int i = 0; i < 1024; i++) {
					bench_to_gui.send(&specs.storageDevices[i], sizeof(char), 0);
				}
				bench_to_gui.send(&specs.processorCoreCount, sizeof(DWORD), 0);
				bench_to_gui.send(&specs.logicalProcessorCount, sizeof(DWORD), 0);
				bench_to_gui.send(&specs.processorL1CacheCount, sizeof(DWORD), 0);
				bench_to_gui.send(&specs.processorL2CacheCount, sizeof(DWORD), 0);
				bench_to_gui.send(&specs.processorL3CacheCount, sizeof(DWORD), 0);
				bench_to_gui.send(&specs.totalMemory, sizeof(int), 0);
				bench_to_gui.send(&specs.availableMemory, sizeof(int), 0);
				bench_to_gui.send(&specs.memoryLength, sizeof(int), 0);
				bench_to_gui.send(&specs.pageSize, sizeof(int), 0);
				bench_to_gui.send(&specs.nominalFrequency, sizeof(int), 0);
			}
			catch (interprocess_exception& ex) {
				//std::cout << ex.what() << std::endl;
			}
			break;
		}
		case 0:																					/////////////////////////////////////////////////////Prime Int
		{
			//std::cout << "Got " << 0 << " as aglorithm index." << std::endl;
			gui_to_bench.receive(&threads, sizeof(int), recvd_size, priority);
			gui_to_bench.receive(&seconds, sizeof(int), recvd_size, priority);
			//std::cout << "Starting PrimeInteger on " << threads << " threads for " << seconds << " seconds." << std::endl;
			PrimeInteger pi;
			pi.run_for(threads, seconds);
			break;
		}
		case 1:																					/////////////////////////////////////////////////////Prime Float
		{
			//std::cout << "Got " << 1 << " as aglorithm index." << std::endl;
			gui_to_bench.receive(&threads, sizeof(int), recvd_size, priority);
			gui_to_bench.receive(&seconds, sizeof(int), recvd_size, priority);
			//std::cout << "Starting PrimeFloat on " << threads << " threads for " << seconds << " seconds." << std::endl;
			PrimeFloat pf;
			pf.run_for(threads, seconds);
			break;
		}
		case 2:																					/////////////////////////////////////////////////////Sha256
		{
			//std::cout << "Got " << 2 << " as aglorithm index." << std::endl;
			gui_to_bench.receive(&threads, sizeof(int), recvd_size, priority);
			gui_to_bench.receive(&seconds, sizeof(int), recvd_size, priority);
			//std::cout << "Starting SHA256 on " << threads << " threads for " << seconds << " seconds." << std::endl;
			SHA256 s;
			s.run_for(threads, seconds);
			break;
		}
		case 3:																					/////////////////////////////////////////////////////-
		{
			//std::cout << "Got " << 3 << " as aglorithm index. Doing nothing..." << std::endl;
			break;
		}
		case 4:																					/////////////////////////////////////////////////////-
		{
			//std::cout << "Got " << 4 << " as aglorithm index. Doing memory test..." << std::endl;
			
			break;
		}
		case 5:
		{
			////std::cout << "Sending realtime data... ";
			try {
				message_queue bench_to_gui(open_only, "BENCH_TO_GUI");
				int availableMemory = specs.getAvailableMemory();
				int usage = specs.getUsage2();
				int frequency = specs.getFrequency();
				bench_to_gui.send(&usage, sizeof(int), 0);
				bench_to_gui.send(&frequency, sizeof(int), 0);
				bench_to_gui.send(&availableMemory, sizeof(int), 0);
			}
			catch (interprocess_exception& ex) {
				//std::cout << ex.what() << std::endl;
			}
			////std::cout << "Done sending. Waiting 300ms." << std::endl;
			Sleep(300);
			break;
		}
		case 6:
		{
			//std::cout << "Testing memory... " << std::endl;
			gui_to_bench.receive(&allocation_size, sizeof(int), recvd_size, priority);
			allocation_size /= 8;
			try {
				long long int avg = 0;
				for (int i = 0; i < 20; i++)
					avg += test(allocation_size);
				avg /= 20;
				//std::cout << "Avg nanosecond duration: " << avg << std::endl;
				message_queue bench_to_gui(open_only, "BENCH_TO_GUI");
				bench_to_gui.send(&avg, sizeof(int), 0);
			}
			catch (interprocess_exception& ex) {
				//std::cout << ex.what() << std::endl;
			}
			//std::cout << "Done testing." << std::endl;
			break;
		}
		}
	}
	catch (interprocess_exception& ex) {
		//std::cout << ex.what() << std::endl;
		exit(-9);
	}


	return 0;
}