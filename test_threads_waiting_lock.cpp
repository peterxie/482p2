#include "thread.h"
#include <iostream>

using namespace std;

// This tests behavior of parent thread finishing before child thread,
// and that mutex is given FIFO-style

// Expected output:
// parent thread has lock
// child 0 running
// child 1 running
// child 2 running
// parent thread finishing
// child 0 has lock
// child 1 has lock
// child 2 has lock
// All CPUs suspended.  Exiting.

mutex m1;

void child(void *a)
{
	intptr_t id = (intptr_t) a;
	
	cout << "child " << id << " running" << endl;
	
	m1.lock();

	cout << "child " << id << " has lock" << endl;
	
	m1.unlock();
}

void parent(void *a)
{
	char *id = (char *) a;
	
	for(int i = 0; i < 3; ++i) {
		thread((thread_startfunc_t) child, (void *) (intptr_t) i);
	}
	
	m1.lock();
	
	cout << id << " has lock" << endl;
	
	for(int i = 0; i < 3; ++i) {
		thread::yield();
	}
	
	cout << id << " finishing" << endl;

	m1.unlock();
}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}
