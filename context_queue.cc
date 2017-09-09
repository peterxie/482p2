#include "context_queue.h"
#include "global.h"
#include "cpu.h"
#include "cpu_impl.h"
#include <exception>

using std::list;

void ready_q::push(context_wrapper* ucp_w)
{
  assert_interrupts_disabled();
  std::list<context_wrapper*>::push_back(ucp_w);
  wake_cpus();
}

void ready_q::pop() {
  assert_interrupts_disabled();
  list<context_wrapper*>::pop_front();
}

void ready_q::splice(cw_list::iterator position, cw_list& x)
{
  assert_interrupts_disabled();
  list<context_wrapper*>::splice(position, x);
  wake_cpus();
}

void ready_q::splice(cw_list::iterator position, cw_list& x, cw_list::iterator i)
{
  assert_interrupts_disabled();
  list<context_wrapper*>::splice(position, x, i);
  wake_cpus();
}


void ready_q::wake_cpus()
{
  // Check if there are any sleeping cpus to wake up.
  for(unsigned int i = 0; !processorQ.empty() && i < size(); ++i) {
    cpu* waiting_cpu_ptr = processorQ.front();
    processorQ.pop();
    waiting_cpu_ptr->interrupt_send();
  }
}



void processor_queue::pop()
{
  assert_interrupts_disabled();
  // Note: we lose the pointer to the original front here.
  // This is what we want, processor_queue is not responsible
  // for memory management, cpu handles this.
  frnt = frnt->impl_ptr->next;
  --sz;
}

void processor_queue::push(cpu* cpu_ptr)
{
  assert_interrupts_disabled();
  // Add ourself to the readyQ.
  if(sz == 0)
    frnt = bck = cpu_ptr;
  else
    bck->impl_ptr->next = cpu_ptr;

  cpu_ptr->impl_ptr->next = nullptr;
  ++sz;
}
