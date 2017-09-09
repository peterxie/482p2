#include "thread.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

using namespace std;

mutex mutex1;
cv cv1;

char* g;

void yielder(void* arg)
{
  mutex1.lock();
  g = (char*)arg;
  thread::yield();
  cv1.wait(mutex1);
  thread::yield();
  mutex1.unlock();
}

void parent(void *a)
{
  int arg = *((int*) a);

  cout << "parent called with arg " << arg << endl;
  assert_interrupts_enabled();
  try {
    while(true)
  	  thread t1 ( (thread_startfunc_t) yielder, (void *) "yielding");
  } catch(bad_alloc& e) {
    cout << "Memory used up from spawning child threads.\n";
  }
  
  cout << "Now yield to all threads and make sure this does not throw\n";
  try {
    thread::yield();
    cv1.broadcast();
    thread::yield();
    mutex1.lock();
    thread::yield();
    mutex1.unlock();
  } catch(bad_alloc& e) {
    cout << "ERROR: thread yield, lock, unlock, or wait threw a bad_alloc\n";
  }
  

}

int main()
{
	int x = 100;
	cpu::boot(1, (thread_startfunc_t) parent, (void *) &x, false, false, 0);
}
