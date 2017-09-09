#include "context_wrapper.h"
#include "global.h"
#include "thread.h"
#include "thread_impl.h"


context_wrapper::context_wrapper(thread_startfunc_t func, void *arg, thread* t) {
  owner = t;
  assert_interrupts_disabled();
  ucp = new ucontext_t();
  safe_getcontext(ucp);

  char *stack = new char[STACK_SIZE];
  ucp->uc_stack.ss_sp = stack;
  ucp->uc_stack.ss_size = STACK_SIZE;
  ucp->uc_stack.ss_flags = 0;
  ucp->uc_link = nullptr;

  makecontext(ucp, (void (*)(void)) start_func,2, func,  arg);

}

context_wrapper::~context_wrapper() {
  assert_interrupts_disabled();
  guard_up();                                 //lock
  if(owner) owner->impl_ptr->ucp_weak = nullptr;
  readyQ.splice(readyQ.end(), waiting_to_finish);
  guard_down();                               //unlock
  char* stack_ptr = static_cast<char*>(ucp->uc_stack.ss_sp);
  delete[] stack_ptr;
  delete ucp;

}


