#include<iostream>
#include "thread.h"

using namespace std;

mutex mutex1;
cv cv1;
thread* t_ptr;

int child_done = 0;

void child(void *a)
{
  char *message = (char*) a;
  mutex1.lock();
  cout << "child run with message " << message << ", setting child_done = 1\n";
  child_done = 1;
  t_ptr->join();
  cv1.signal();
  mutex1.unlock();
}

void parent(void *a)
{
  intptr_t arg = (intptr_t) a;
  cout << "parent called with arg " << arg << endl;
  thread t1 ((thread_startfunc_t) child, (void*) "test message");
  mutex1.lock();
  t_ptr = &t1;
  while (!child_done) {
    cout << "parent waiting for child to run\n";
    cv1.wait(mutex1);
  }
  cout << "parent finishing" << endl;
  mutex1.unlock();
}

int main()
{
  cpu::boot(1, (thread_startfunc_t) parent, (void*) 100, 0, 0, 0);
}
