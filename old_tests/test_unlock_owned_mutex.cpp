#include "thread.h"
#include <iostream>

using namespace std;

// This tests behavior of a thread unlocking a mutex
// owned by another thread

// Expected output:
// parent thread has lock
// child thread running
// terminate called after throwing an instance of 'std::runtime_error'
//   what():  Thread cannot unlock mutex it does not own.
// Aborted


mutex m1;

void child(void *a)
{
	char *id = (char *) a;
	
	cout << id << " running" << endl;
	
	m1.unlock();
	
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
	
	thread::yield();
	
	cout << id << " finishing" << endl;
	
	m1.unlock();

}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}
