#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <atomic>
#include <memory>
#include <list>
#include "context_wrapper.h"
#include "context_queue.h"

extern std::atomic<bool> guard;
extern ready_q readyQ;    																			//waiting to execute
extern processor_queue processorQ;


void guard_up();
void guard_down();

void safe_getcontext(ucontext_t* ucp);

void safe_getcontext(context_wrapper* ucp_w);
void safe_setcontext(context_wrapper* ucp_w);
void safe_swapcontext(ucontext_t* cur, ucontext_t* next);
void safe_swapcontext(context_wrapper* cur, context_wrapper* next);


void start_func(thread_startfunc_t func, void *arg);

#endif
