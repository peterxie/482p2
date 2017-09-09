#include "thread.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// This tests behavior of a thread unlocking a mutex
// owned by no thread

// Expected output:
// terminate called after throwing an instance of 'std::runtime_error'
//   what():  Thread cannot unlock mutex it does not own.
// Aborted


mutex m1;

void parent(void *a)
{
  try{
    char *id = (char *) a;
	
    m1.unlock();

    cout << id << " finishing" << endl;
  }
  catch (runtime_error &e) {
    cout << e.what() << endl;
  }
}


int main()
{
	cpu::boot(1, (thread_startfunc_t) parent, (void *) "parent thread", false, false, 0);
}
