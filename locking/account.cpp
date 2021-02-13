#include <cstdio>
#include <string>

#include "account.h"

using std::string;
using std::unique_lock;
using std::mutex;

void Account::add(unsigned int tpounds, unsigned int tpence)
{
	unique_lock<mutex> lock(money_mutex);
	pounds_ += tpounds;
	pence_ += tpence;

	// Ensure pence_ stays in the range 0-99.
	if (pence_ >= 100) {
		pounds_++;
		pence_ -= 100;
	}
}

string Account::total()
{
	char buf[40];
	unique_lock<mutex> lock(money_mutex);
	snprintf(buf, sizeof buf, "%u.%02u", pounds_, pence_);
	lock.unlock();
	return string(buf);
}