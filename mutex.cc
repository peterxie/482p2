#include "mutex.h"
#include "mutex_impl.h"
#include "cpu.h"
#include "cpu_impl.h"
#include "global.h"
#include <stdexcept>
#include <exception>


using std::shared_ptr;
using std::runtime_error;
extern ready_q readyQ;

/* TODO IF SOMEBODY DOES NOT HAVE THE LOCK
   AND THEY TRY TO LOCK, THROW AN ERROR */

mutex::mutex()
{
	try {
		impl_ptr = new mutex::impl{};
	}
	
	catch(std::bad_alloc &e) {
		throw e;
	}

}

mutex::~mutex()
{ delete mutex::impl_ptr; }

void mutex::lock() {
  assert_interrupts_enabled();
	cpu::interrupt_disable();
	guard_up();
	
  impl_ptr->lock_helper();
	
	guard_down();
	cpu::interrupt_enable();
}


void mutex::impl::lock_helper() {
  assert_interrupts_disabled();

  context_wrapper* cur_job = cpu::self()->impl_ptr->running_context.front();

  if(free) {
    free = false;
    owner = cur_job;
  }
  
  else {
    mutexQ.splice(mutexQ.end(), cpu::self()->impl_ptr->running_context, cpu::self()->impl_ptr->running_context.begin());
  //return control to cpu
    safe_swapcontext(cur_job, cpu::self()->impl_ptr->cpu_context);
  }
}

void mutex::unlock() {
  assert_interrupts_enabled();
	cpu::interrupt_disable();
	guard_up();

  impl_ptr->unlock_helper();

	guard_down();
  cpu::interrupt_enable();
}

void mutex::impl::unlock_helper() {
  assert_interrupts_disabled();
  // If the current context is not the owner, throw error.
  if(cpu::self()->impl_ptr->running_context.front() != owner) {
    cpu::interrupt_enable();
    guard_down();
    throw runtime_error{"Thread cannot unlock mutex it does not own."};
  }
  
  free = true;
  
  if(!mutexQ.empty()) { 
    owner = mutexQ.front();
    readyQ.splice(readyQ.end(), mutexQ, mutexQ.begin());
    free = false;
  } 
  else {
    owner = nullptr;
  }
}
