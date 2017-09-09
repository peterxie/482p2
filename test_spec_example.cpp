#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int g = 0;

mutex mutex1;
cv cv1;

void loop(void *a)
{
	char *id = (char *) a;
	int i;

	cout << "loop called with id " << id << endl;
  cv1.signal();
  cv1.broadcast();
	mutex1.lock();
  thread::yield();
	for (i=0; i<5; i++, g++) {
		cout << id << ":\t" << i << "\t" << g << endl;
    cv1.wait(mutex1);
		mutex1.unlock();
		thread::yield();
		mutex1.lock();
	}
	cout << id << ":\t" << i << "\t" << g << endl;
	mutex1.unlock();
}

void parent(void *a)
{
	intptr_t arg = (intptr_t) a;

	cout << "parent called with arg " << arg << endl;
	assert_interrupts_enabled();
	for(int i = 0; i < 10; ++i)
    thread t1 ( (thread_startfunc_t) loop, (void *) "child thread");
  thread::yield();

  mutex1.lock();
  while(g < 49) {
    cv1.signal();
    thread::yield();
    mutex1.unlock();
    mutex1.lock();
  }
}

int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}
