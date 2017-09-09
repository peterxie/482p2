#include "thread.h"
#include <iostream>

using namespace std;

// This tests behavior of parent thread finishing before child thread,
// and that mutex is given FIFO-style

// Expected output:

mutex m1;
mutex m2;
cv cv1;
int counter = 0;

void child(void *a)
{
	intptr_t id = (intptr_t) a;
	
	m1.lock();
		
	++counter;
	cout << "child " << id << " started" << endl;
	
	while(counter < 6) {
		cv1.wait(m1);
	}
	
	cout << "child " << id << " finishing" << endl;
	
	m1.unlock();
}

void parent(void *a)
{
	char *id = (char *) a;
	
	for(int i = 0; i < 5; i++) {
		thread((thread_startfunc_t) child, (void *) (intptr_t) i);
	}
	
	cout << id << " started" << endl;
	
	thread::yield();
	
	m1.lock();
	cout << id << " has lock" << endl;
	
	++counter;
	cv1.broadcast();

	cout << id << " finishing" << endl;

	m1.unlock();
}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}
