#ifndef CPU_IMPL_H_
#define CPU_IMPL_H_

#include "cpu.h"
#include "context_queue.h"
#include "context_wrapper.h"
#include <list>
#include <memory>


void IPI_handler();


class cpu::impl {
  
  friend cpu;
  friend thread; 
  friend cv;
  friend mutex;
  friend processor_queue;

public:
	
  impl()
  :next(nullptr), 
   cpu_context(new context_wrapper(nullptr, nullptr, nullptr)),
   context_is_finished(false)
  {}
    
  ~impl()
  {}
    
  friend void timer_handler();
  friend void start_func(thread_startfunc_t func, void *arg);
private:
  cpu* next;
  std::list<context_wrapper*> running_context;
  context_wrapper* cpu_context;
  bool context_is_finished;
};


#endif
