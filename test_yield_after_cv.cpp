#include "thread.h"
#include <iostream>

using namespace std;

// This tests behavior basic wait CV procedure,
// and ensures that a waiting thread will not run upon yield

// Expected output:

mutex m1;
cv cv1;
bool flag = false;

void child(void *a)
{
	char *id = (char *) a;
	
	cout << id << " running" << endl;
	
	m1.lock();
	
	cout << id << " has lock" << endl;
	flag = true;
	thread::yield();
	cv1.signal();

	m1.unlock();
}

void parent(void *a)
{
	char *id = (char *) a;
	
	thread t1((thread_startfunc_t) child, (void *) a);
	
	m1.lock();
	
	cout << id << " has lock" << endl;
	
	while(!flag) {
		cv1.wait(m1);
	}
	
	cout << id << " finishing" << endl;

	m1.unlock();
}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}
