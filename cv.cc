#include "cv_impl.h"
#include "mutex.h"
#include "cpu.h"
#include "global.h"
#include "cpu_impl.h"
#include "mutex_impl.h"
#include <exception>

using std::shared_ptr;

cv::cv()
{
	try {
		impl_ptr = new cv::impl{};
	}
	
	catch(std::bad_alloc &e) {
		throw e;
	}

}

cv::~cv()
{ delete cv::impl_ptr; }

void cv::wait(mutex& m) {
	
	//NEED TO CHECK THAT THREAD HOLDS LOCK BEFORE CALLING THIS
	//could unlock() return this error for us?
	cpu::interrupt_disable();
	guard_up();

	m.impl_ptr->unlock_helper();
		
	// push CPU's active context onto the queue for this CV
	context_wrapper* curr_context = cpu::self()->impl_ptr->running_context.front();
  impl_ptr->cvQ.splice(impl_ptr->cvQ.end(), cpu::self()->impl_ptr->running_context, cpu::self()->impl_ptr->running_context.begin());
	// give control back to CPU
	safe_swapcontext(curr_context, cpu::self()->impl_ptr->cpu_context);

	m.impl_ptr->lock_helper();

	guard_down();
	cpu::interrupt_enable();
	
}

void cv::signal() {
	cpu::interrupt_disable();
	guard_up();
	
	if(!impl_ptr->cvQ.empty()) {
		readyQ.splice(readyQ.end(), impl_ptr->cvQ, impl_ptr->cvQ.begin());
	}
	
	guard_down();
	cpu::interrupt_enable();
}

void cv::broadcast() {
	cpu::interrupt_disable();
	guard_up();
	
	while(!impl_ptr->cvQ.empty()) {
		
		readyQ.splice(readyQ.end(),impl_ptr->cvQ, impl_ptr->cvQ.begin());
	}
	
	guard_down();
	cpu::interrupt_enable();
}




