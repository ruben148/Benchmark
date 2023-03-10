#pragma once
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <random>
#include <time.h>

#define x5(x)  x x x x x
#define x10(x)  x5(x) x5(x)
#define x20(x)  x10(x) x10(x)
#define x40(x)  x20(x) x20(x)
#define x80(x)  x40(x) x40(x)
#define x160(x)  x80(x) x80(x)
#define x360(x)  x160(x) x160(x)
#define x720(x)  x360(x) x360(x)

unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
	a = a - b;  a = a - c;  a = a ^ (c >> 13);
	b = b - c;  b = b - a;  b = b ^ (a << 8);
	c = c - a;  c = c - b;  c = c ^ (b >> 13);
	a = a - b;  a = a - c;  a = a ^ (c >> 12);
	b = b - c;  b = b - a;  b = b ^ (a << 16);
	c = c - a;  c = c - b;  c = c ^ (b >> 5);
	a = a - b;  a = a - c;  a = a ^ (c >> 3);
	b = b - c;  b = b - a;  b = b ^ (a << 10);
	c = c - a;  c = c - b;  c = c ^ (b >> 15);
	return c;
}

void perfectShuffle(long long int *array, int size)
{
	srand(mix(clock(), time(NULL), _getpid()));

	for (int i = 0; i < size; i++)
	{
		int j = rand() % (size - i) + i;

		long long int temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}
}

void linearAccess(long long int *array, long long int a, long long int b, int stride)
{

}

long long int test(int allocation_size) {

	void **pp = (void**)malloc(allocation_size * sizeof(void*)); //8 bytes per pointer
	//std::cout << "Allocated " << allocation_size * sizeof(void*) << " bytes (" << allocation_size * sizeof(void*) / 1024 << " KB)" << std::endl;
	void **p = pp;
	long long int *q = (long long int*) p;

	long long int start_address = reinterpret_cast<long long int> (pp);

	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<long long int> dist(((long long int)p)/8, (((long long int)p)+allocation_size)/8);

	for (int i = 0; i < allocation_size; i++)
	{
		q[i] = start_address + i * 8;
	}

	perfectShuffle(q, allocation_size);

	long long int ad1 = start_address, ad2 = start_address;
	long long int diff = 0;
	long long int total_diff = 0;
	/*
	for (int i = 0; i < allocation_size; i++)
	{
		ad1 = q[(ad1 - start_address)/8];
		diff = ad1 - ad2;
		total_diff += abs(diff);
		ad2 = ad1;
		//std::cout << "AD1 = " << std::hex << ad1 << std::endl;
		//std::cout << "start_address = " << std::hex << start_address << std::endl;
		//std::cout << "ad1 - start_address = " << std::hex << ad1 - start_address << std::endl;
	}
	std::cout << std::dec << "Total difference = " << total_diff << std::endl;
	*/
	begin = std::chrono::steady_clock::now();
	
	for (int i = 0; i < 100; i++)
		x720(p = (void**)*p;)

	end = std::chrono::steady_clock::now();
	long long int *number = (long long int *)*p;
	std::cout << "Last number is " << *number << std::endl;
	long long int time_difference = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << std::dec << "Access time = " << time_difference << "nanoseconds" << std::endl;
	//std::cout << "Total address difference: " << total_address_difference << std::endl << std::endl;

	free(pp);
	return time_difference;
}