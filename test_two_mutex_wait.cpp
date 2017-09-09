#include "thread.h"
#include <iostream>

using namespace std;

// This tests behavior of two threads both waiting on a CV

mutex m1;
mutex m2;
cv cv1;
bool flag = false;
bool child_go = false;
bool parent_go = false;

void child2(void *a)
{
  char *id = (char *) a;
  cout << id << " running" << endl;
  m1.lock();
  m2.lock();

  child_go = true;
  parent_go = true;

  m1.unlock();
  m2.unlock();
  cv1.broadcast();
}

void child(void *a)
{
	char *id = (char *) a;
	
	cout << id << " running" << endl;
	
	m2.lock();
	
	cout << id << " has lock m2" << endl;

	while(!child_go)cv1.wait(m2);

	m2.unlock();
  cv1.signal();
}

void parent(void *a)
{
	char *id = (char *) a;
	
	thread t1((thread_startfunc_t) child, (void *) "child thread");
	thread t2((thread_startfunc_t) child2, (void*) "child2 thread");
	m1.lock();
	
	cout << id << " has lock m1" << endl;
	
	while(!parent_go)cv1.wait(m1);
	
	cout << id << " finishing" << endl;

	m1.unlock();
  cv1.signal();
}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}
