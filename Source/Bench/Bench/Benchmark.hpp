#pragma once
#include <boost/interprocess/ipc/message_queue.hpp>
#include "Specs.hpp"
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <algorithm>
#include <queue>
#include <deque>

using namespace boost::interprocess;

namespace benchmark
{
	template<typename T, int MaxLen, typename Container = std::deque<T> >
	class FixedQueue : public std::queue<T, Container>
	{
	public:
		typedef typename Container::iterator iterator;
		typedef typename Container::const_iterator const_iterator;
		bool full = false;
		void push(const T& value)
		{
			if (this->size() == MaxLen)
			{
				full = true;
				this->c.pop_front();
			}
			std::queue<T, Container>::push(value);
		}
		int average()
		{
			int avg = 0;
			for (auto it = this->begin(); it != this->end(); ++it)
				avg += *it;
			avg /= this->size();
			return avg;
		}
		iterator begin() { return this->c.begin(); }
		iterator end() { return this->c.end(); }
		const_iterator begin() const { return this->c.begin(); }
		const_iterator end() const { return this->c.end(); }
	};

	class BenchmarkAlgorithm
	{
	public:
		void run_for(int number_of_threads, int seconds);
		void run_multithreaded(std::vector<std::thread>& thread_pool, bool join);
	protected:
		virtual void run(std::vector<std::thread>& thread_pool) = 0;
		void initialize_args(int number_of_threads);
		int get_score(int duration, int xValue, int number_of_increments);
		double get_xValue(int duration, double min, double max);
		std::vector<long double> complexity_polynomial;
		int* results;
		bool running;
		int upper_limit_index;
		bool lower_limit;
		int lower_limit_index;
		bool increment_lower_limit;
		bool increment_upper_limit;
		int increment;
		int max_upper_limit;
		int starting_upper_limit;
		std::vector<int> args;
		int number_of_increments;
	};
	class PrimeInteger : public BenchmarkAlgorithm
	{
	public:
		PrimeInteger()
		{
			lower_limit = true;
			increment_lower_limit = true;
			increment_upper_limit = true;
			lower_limit_index = 1;
			upper_limit_index = 2;
			increment = 50000;
			max_upper_limit = 24650000;
			complexity_polynomial = {
										2.013591588395560E-45	,
										-1.919951887815270E-37	,
										7.455510837806760E-30	,
										-1.519526093653800E-22	,
										1.755283658939030E-15	,
										-1.186298392870750E-08	,
										5.876447454235120E-02	,
										2.188044727581620E+04 };
		};
	private:
		void run(std::vector<std::thread>& thread_pool);
		void thread_function(std::vector<int> q, int* number_of_primes, int* time_difference);
		bool is_prime(int a);
	};
	class PrimeFloat : public BenchmarkAlgorithm
	{
	public:
		PrimeFloat()
		{
			lower_limit = true;
			increment_lower_limit = true;
			increment_upper_limit = true;
			lower_limit_index = 1;
			upper_limit_index = 2;
			increment = 50000;			//TO BE DETERMINED
			max_upper_limit = 7950000;
			complexity_polynomial = {
										1.7386642862462100E-41  ,
										-5.3501444088718800E-34 ,
										6.7079291820407500E-27  ,
										-4.4167690267817400E-20 ,
										1.6494069369378500E-13  ,
										-3.6050643681590800E-07 ,
										5.7388102862244700E-01  ,
										6.2342932968287700E+04 };
		};
	private:
		void run(std::vector<std::thread>& thread_pool);
		void thread_function(std::vector<double> q, int* number_of_primes, int* time_difference);
		bool is_prime(double a);
	};
	class SHA256 : public BenchmarkAlgorithm
	{

#define uchar unsigned char
#define uint unsigned int

#define DBL_INT_ADD(a,b,c) if (a > 0xffffffff - (c)) ++b; a += c;
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

		typedef struct {
			uchar data[64];
			uint datalen;
			uint bitlen[2];
			uint state[8];
		} SHA256_CTX;
		uint k[64] = {
			0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
			0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
			0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
			0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
			0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
			0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
			0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
			0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
		};
	public:
		SHA256()
		{
			lower_limit = false;
			increment_lower_limit = false;
			increment_upper_limit = false;
			lower_limit_index = -1;
			upper_limit_index = 1;
			increment = 200000;
			max_upper_limit = INT_MAX;
			complexity_polynomial = { 440000 };
		};
	private:
		void SHA256Transform(SHA256_CTX* ctx, uchar data[]);
		void SHA256Init(SHA256_CTX* ctx);
		void SHA256Update(SHA256_CTX* ctx, uchar data[], uint len);
		void SHA256Final(SHA256_CTX* ctx, uchar hash[]);
		void compute(char* data, std::string& hashStr);
		void run(std::vector<std::thread>& thread_pool);
		void thread_function(int lenght, int hashes, int* time_difference);
	};
	class Compression : private BenchmarkAlgorithm
	{
	public:
		Compression() {

		};
		void run(std::vector<std::thread>& thread_pool, std::vector<int> args, int*& results);
		void thread_function(std::vector<std::thread>& thread_pool, std::vector<int> args, int*& results);
	};

	class Benchmark
	{
	public:
		void run_for(BenchmarkAlgorithm& algo, int number_of_threads, int seconds);
	};
}