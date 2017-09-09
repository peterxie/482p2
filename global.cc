#include "global.h"
#include "cpu.h"
#include "cpu_impl.h"
#include <cassert>
#include "context_queue.h"
//#define NDEBUG


ready_q readyQ;
processor_queue processorQ;

void guard_up() {
  while(guard.exchange(true));
}

void guard_down() {
  guard.store(false);
}

void safe_getcontext(ucontext_t* ucp)
{
  int error = getcontext(ucp);
  assert(error == 0);
}

void safe_getcontext(context_wrapper* ucp_w)
{
  int error = getcontext(ucp_w->ucp);
  assert(error == 0);
}

void safe_setcontext(context_wrapper* ucp_w)
{
  int error = setcontext(ucp_w->ucp);
  assert(error == 0);
}


void safe_swapcontext(ucontext_t* cur, ucontext_t* next)
{
  assert_interrupts_disabled();
  int error = swapcontext(cur, next);
  assert(error == 0);
  assert_interrupts_disabled();
}

void safe_swapcontext(context_wrapper* cur, context_wrapper* next)
{
  assert(cur);
  assert(next);
  safe_swapcontext(cur->ucp, next->ucp);
}

void start_func(thread_startfunc_t func, void *arg) {
  cpu::interrupt_enable();
  guard_down();
  (*func)(arg);
  cpu::interrupt_disable();   
  guard_up();
  // Set context_is_finished so that the context_pointer is
  // deleted when the context switches back to the cpu
  cpu::self()->impl_ptr->context_is_finished = true;
  assert_interrupts_disabled();
  ucontext_t temp;
  safe_swapcontext(&temp, cpu::self()->impl_ptr->cpu_context->ucp);

}
