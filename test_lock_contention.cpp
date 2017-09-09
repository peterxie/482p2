#include "thread.h"
#include <iostream>

using namespace std;

// As discussed in lecture, this tests whether the thread library
// correctly implements contention over a lock by multiple threads

// Expected output:
// parent thread has lock
// parent finishing
// child thread has lock

mutex m1;

void child(void *a)
{
	char *id = (char *) a;
	
	m1.lock();

	cout << id << " has lock" << endl;
	
	m1.unlock();
}

void parent(void *a)
{
	char *id = (char *) a;
	
	thread t1 ( (thread_startfunc_t) child, (void *) "child thread");

	m1.lock();
	
	cout << id << " has lock" << endl;
	
	thread::yield();
	
	cout << id << " finishing" << endl;

	m1.unlock();
}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}

