#include "thread.h"
#include <iostream>

using namespace std;

// This tests behavior of a thread finishing without releasing lock
// that other thread waits on

// Expected output:
// parent thread has lock
// child thread running
// All CPUs suspended.  Exiting.

mutex m1;

void child(void *a)
{
	char *id = (char *) a;
	
	cout << id << " running" << endl;
	
	m1.lock();

	cout << id << " finishing" << endl;
	
	m1.unlock();
}

void parent(void *a)
{
	char *id = (char *) a;
	
	thread t1((thread_startfunc_t) child, (void *) "child thread");
	
	m1.lock();
	
	cout << id << " has lock" << endl;

}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}
