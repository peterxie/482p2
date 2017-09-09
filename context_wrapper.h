#ifndef CONTEXT_WRAPPER_H_
#define CONTEXT_WRAPPER_H_

#include <ucontext.h>
#include <vector>
#include <memory>
#include "thread.h"
#include <list>

class context_wrapper {
	public:

		context_wrapper(thread_startfunc_t func, void *arg, thread* t);
		~context_wrapper();

    std::list<context_wrapper*> waiting_to_finish;
		ucontext_t *ucp;
    thread *owner;
};



#endif

