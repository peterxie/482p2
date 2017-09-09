#include<iostream>
#include "thread.h"

using namespace std;

mutex mutex1;
cv cv1;

int child_done = 0;

void child(void *a)
{
  char *message = (char*) a;
  mutex1.lock();
  cout << "child run with message " << message << ", setting child_done = 1\n";
  child_done = 1;
  mutex1.unlock();
}

void parent(void *a)
{
  intptr_t arg = (intptr_t) a;
  mutex1.lock();
  cout << "parent called with arg " << arg << endl;
  mutex1.unlock();
  thread t1 ((thread_startfunc_t) child, (void*) "test message");

  mutex1.lock();
  cout << "yielding thread\n";
  mutex1.unlock();
  thread::yield();

  mutex1.lock();
  cout << "creating second child\n";
  thread t2 ((thread_startfunc_t) child, (void*) "child 2");
  cout << "joining on finished child thread\n";
  t1.join();
  cout << "parent finishing" << endl;
  mutex1.unlock();
}

void real_parent(void *a)
{
  intptr_t arg = (intptr_t) a;
  mutex1.lock();
  cout << "real parent called with arg " << arg << endl;
  mutex1.unlock();

  thread t ((thread_startfunc_t) parent, (void*) 1);
  cout << "joining on parent thread\n";
  t.join();
  return;
}

int main()
{
  cpu::boot(1, (thread_startfunc_t) real_parent, (void*) 100, 0, 0, 0);
}
