#include <iostream>
#include "thread.h"

using namespace std;


/* expected output:
spawning child
Wait for child
Child counting to 10
1
2
3
4
5
6
7
8
9
10
Child yielding
Child finishing
Parent finishing
*/

void child(void *)
{
  cout << "Child counting to 10\n";
  for(auto i = 1; i <= 10; ++i) {
    cout << i << endl;
  }
  cout << "Child yielding\n";
  thread::yield();
  cout << "Child finishing\n";
}

void parent(void *)
{
  cout << "spawning child\n";
  thread child_thread((thread_startfunc_t) child, nullptr);
  cout << "Wait for child\n";
  child_thread.join();
  cout << "Parent finishing\n";
}


int main()
{
  cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}


