#include "thread.h"
#include "thread_impl.h"
#include "cpu.h"
#include "global.h"
#include "cpu_impl.h"
#include <ucontext.h>
#include <memory>
#include <exception>

thread::thread(thread_startfunc_t func, void *arg) // create a new thread
{
  assert_interrupts_enabled();
  cpu::interrupt_disable();
  
  try {
		// Create a new ucontext object to save the current cpu ucontext
		context_wrapper* ucp_w( new context_wrapper(func, arg, this));
		impl_ptr = new thread::impl(ucp_w); 
	

	
    // Add this new context to the readyQ
    guard_up();
    readyQ.push(ucp_w);
    guard_down();
    cpu::interrupt_enable();
  }
	catch(std::bad_alloc &e) {
		cpu::interrupt_enable();
		throw e;
	}
}

thread::~thread() {
  if(impl_ptr->ucp_weak) impl_ptr->ucp_weak->owner = nullptr;
  delete impl_ptr;
}


void thread::yield()
{
  assert_interrupts_enabled();
  cpu::interrupt_disable();
	guard_up();

  //get the current running context from this cpu and put it on the ready queue
  context_wrapper* curr_job_ucp_w = cpu::self()->impl_ptr->running_context.front();
	readyQ.splice(readyQ.end(), cpu::self()->impl_ptr->running_context,cpu::self()->impl_ptr->running_context.begin());
	
	safe_swapcontext(curr_job_ucp_w, cpu::self()->impl_ptr->cpu_context);
	
	guard_down();
  cpu::interrupt_enable();
}

void thread::join()
{
  assert_interrupts_enabled();
  cpu::interrupt_disable();
  guard_up();                                                     //lock
  if(impl_ptr->ucp_weak) {
    context_wrapper* cur_job = cpu::self()->impl_ptr->running_context.front();

    context_wrapper* joined_thread = impl_ptr->ucp_weak;
    joined_thread->waiting_to_finish.splice(joined_thread->waiting_to_finish.begin(), cpu::self()->impl_ptr->running_context, cpu::self()->impl_ptr->running_context.begin());
    //return control to cpu
    safe_swapcontext(cur_job, cpu::self()->impl_ptr->cpu_context);
    
  }

  guard_down();                                                   //unlock
  cpu::interrupt_enable();
}





