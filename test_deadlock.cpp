#include "thread.h"
#include <iostream>

using namespace std;

// Deterministically deadlocking

// Expected output:
// parent thread acquired lock 1
// child thread acquired lock 2
// All CPUs suspended.  Exiting.

mutex m1;
mutex m2;

void child(void *a)
{
	char *id = (char *) a;
	
	m2.lock();	
	
	cout << id << " acquired lock 2" << endl;
	
	m1.lock();
	
	cout << id << " acquired lock 1, finishing" << endl;
	
	m1.unlock();
	m2.unlock();
}

void parent(void *a)
{
	char *id = (char *) a;
	
	thread t1 ( (thread_startfunc_t) child, (void *) "child thread");

	m1.lock();
	
	cout << id << " acquired lock 1" << endl;

	thread::yield();
	
	m2.lock();
	
	cout << id << " acquired lock 2, finishing" << endl;

	m2.unlock();
	m1.unlock();
}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}

