#include "cpu.h"
#include "global.h"
#include "cpu_impl.h"
#include "cv_impl.h"
#include "context_queue.h"
#include <ucontext.h>
#include <stdexcept>
#include <atomic>
#include <memory>

using std::shared_ptr;
using std::atomic;

extern processor_queue processorQ;
extern ready_q readyQ;

void IPI_handler() {
  assert_interrupts_enabled();
  cpu::interrupt_disable();
}

void timer_handler() {
  assert_interrupts_enabled();
  cpu::interrupt_disable();

  guard_up();
  if(!readyQ.empty()) { 
    context_wrapper* curr_job_ucp_w = cpu::self()->impl_ptr->running_context.front();
    readyQ.splice(readyQ.end(), cpu::self()->impl_ptr->running_context, cpu::self()->impl_ptr->running_context.begin());

    safe_swapcontext(curr_job_ucp_w, cpu::self()->impl_ptr->cpu_context);

  }

  guard_down();
  cpu::interrupt_enable();

}

void cpu::init(thread_startfunc_t func, void * arg) {
  //fill out handler table
  interrupt_vector_table[IPI] = (interrupt_handler_t) IPI_handler;
  interrupt_vector_table[TIMER] = (interrupt_handler_t) timer_handler;

  //initialize cpu::impl
  try {
    impl_ptr = new cpu::impl();
  }
  catch(std::bad_alloc &e){
    cpu::interrupt_enable();
    throw e;
  }
  if(func) {
    cpu::interrupt_enable();
    shared_ptr<thread> t(new thread(func, arg));
    assert_interrupts_enabled();
    cpu::interrupt_disable();
  }
  else {
    guard_up();                                            //lock
    if(readyQ.empty()) {
      processorQ.push(this);
      guard_down();                                          //unlock
      cpu::interrupt_enable_suspend();
      assert_interrupts_disabled();
    }
    else guard_down();
  }

  while(true)
  {
		guard_up();
  
    // Grab a context from the readyQ or sleep
    while(readyQ.empty()) {
      processorQ.push(this);
      guard_down();                                      //unlock
      cpu::interrupt_enable_suspend();
      assert_interrupts_disabled();
      guard_up();                                        //lock
    }


    //do something useful, such as setting or swapping context
    context_wrapper* nextContext = readyQ.front();
    impl_ptr->running_context.splice(impl_ptr->running_context.end(), readyQ, readyQ.begin());

    //start running the thread's context and fill the current context
    //pointer with the details of the current CPU's state of execution

    safe_swapcontext(impl_ptr->cpu_context, nextContext);

    // The context we just returned from is either
    // finished or held on another queue.
    if(impl_ptr->context_is_finished) {
      guard_down();
      context_wrapper* finished_context = impl_ptr->running_context.front();
      delete finished_context;
      impl_ptr->running_context.clear();
      impl_ptr->context_is_finished = false;
      guard_up();
    }
    
    guard_down();  
  }

}

