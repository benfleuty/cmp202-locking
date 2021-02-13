// Race conditions example
// Adam Sampson <a.sampson@abertay.ac.uk>

#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <vector>

#include "account.h"

// Import things we need from the standard library
using std::cout;
using std::endl;
using std::thread;

Account bill;

void bill_add() {
	for (auto i = 0; i < 1000000; ++i)
		// critical
		bill.add(17, 29);
}

int main(int argc, char* argv[])
{
	const uint16_t thread_count = 10;

	const double expected = 1000000 * 17.29 * 10;
	cout << "Expected:\t GBP " << std::fixed << std::setprecision(2) << expected << "\n";

	cout << "Initial:\t GBP "
		<< std::setw(std::to_string(expected).size() - bill.total().size())
		<< bill.total() << "\n";

	std::vector<thread> threads;

	for (auto i = 0; i < thread_count; ++i)
		threads.emplace_back(bill_add);

	for (auto i = 0; i < thread_count; ++i)
		threads[i].join();

	cout << "Total:\t\t GBP "
		<< std::setw(std::to_string(expected).size() - bill.total().size())
		<< bill.total() << "\n";

	return 0;
}