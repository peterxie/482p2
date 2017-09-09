#include "thread.h"
#include <iostream>

using namespace std;

// Deadlocking with one mutex

// Expected output:
// parent thread is about to run horrible code
// All CPUs suspended.  Exiting.

mutex m1;

void parent(void *a)
{
	char *id = (char *) a;

	cout << id << " is about to run horrible code" << endl;

	m1.lock();
	m1.lock();
	
	cout << id << " finally gets to finish" << endl;

	m1.unlock();
	m1.unlock();
}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}
