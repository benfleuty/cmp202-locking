// Race conditions example
// Adam Sampson <a.sampson@abertay.ac.uk>

#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

#include "account.h"

// Import things we need from the standard library
using std::cout;
using std::endl;

using std::vector;

using std::thread;
using std::mutex;
using std::unique_lock;

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::steady_clock;

Account bill;
mutex bill_mutex;

void bill_add()
{
	for (auto i = 0; i < 1000000; ++i)
		bill.add(17, 29);
}

void bill_add_mutex() {
	for (auto i = 0; i < 1000000; ++i)
	{	// critical
		bill_mutex.lock();
		bill.add(17, 29);
		bill_mutex.unlock();
	}
}

void bill_add_mutex_unique() {
	for (auto i = 0; i < 1000000; ++i)
	{	// critical
		unique_lock<mutex> lock(bill_mutex);
		bill.add(17, 29);
	}
}

vector<double> no_locks()
{
	cout << "Starting no_locks()" << endl;
	const auto start = steady_clock::now();

	const uint16_t thread_count = 10;

	std::vector<thread> threads;

	for (auto i = 0; i < thread_count; ++i)
		threads.emplace_back(bill_add);

	for (auto i = 0; i < thread_count; ++i)
		threads[i].join();

	const auto end = steady_clock::now();

	cout << "Finished no_locks()" << endl;

	vector<double> vals;
	// time
	vals.emplace_back(static_cast<double>(duration_cast<milliseconds>(end - start).count()));
	// value
	vals.emplace_back(bill.total());
	return vals;
}

vector<double> bill_add_mutexes()
{
	cout << "Starting bill_add_mutexes()" << endl;
	const auto start = steady_clock::now();

	const uint16_t thread_count = 10;

	std::vector<thread> threads;

	for (auto i = 0; i < thread_count; ++i)
		threads.emplace_back(bill_add_mutex);

	for (auto i = 0; i < thread_count; ++i)
		threads[i].join();

	const auto end = steady_clock::now();
	cout << "Finished bill_add_mutexes()" << endl;

	vector<double> vals;
	// time
	vals.emplace_back(static_cast<double>(duration_cast<milliseconds>(end - start).count()));
	// value
	vals.emplace_back(bill.total());
	return vals;
}

vector<double> unique_lock_mutex() {
	cout << "Starting unique_lock_mutex()" << endl;
	const auto start = steady_clock::now();

	const uint16_t thread_count = 10;

	std::vector<thread> threads;

	for (auto i = 0; i < thread_count; ++i)
		threads.emplace_back(bill_add_mutex_unique);

	for (auto i = 0; i < thread_count; ++i)
		threads[i].join();

	auto end = steady_clock::now();
	cout << "Finished unique_lock_mutex()" << endl;

	vector<double> vals;
	// time
	vals.emplace_back(static_cast<double>(duration_cast<milliseconds>(end - start).count()));
	// value
	vals.emplace_back(bill.total());
	return vals;
}

int main(int argc, char* argv[])
{
	const auto no_locks_time = no_locks();
	bill.reset_total();
	const auto mutexes_time = bill_add_mutexes();
	bill.reset_total();
	const auto unique_lock_time = unique_lock_mutex();

	cout << std::fixed << std::setprecision(2) << endl;
	cout << "No locks:" << endl
		<< "Time:  " << no_locks_time[0] << " ms" << endl
		<< "Value: " << no_locks_time[1] << " GBP" << endl << endl;

	cout << "Mutexes:" << endl
		<< "Time: " << mutexes_time[0] << " ms" << endl
		<< "Value: " << mutexes_time[1] << " GBP" << endl << endl;

	cout << "Unique locks:" << endl
		<< "Time: " << unique_lock_time[0] << " ms" << endl
		<< "Value: " << unique_lock_time[1] << " GBP" << endl << endl;

	return 0;
}