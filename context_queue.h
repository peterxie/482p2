#ifndef _CONTEXT_QUEUE_H
#define _CONTEXT_QUEUE_H

#include "context_wrapper.h"
#include "cpu.h"
#include <list>

using cw_list = std::list<context_wrapper*>;

class ready_q : public cw_list {
public:
  void push(context_wrapper* ucp_w);
  void pop();
  void splice(cw_list::iterator position, cw_list& x);
  void splice(cw_list::iterator position, cw_list& x, cw_list::iterator i);

private:
  void wake_cpus();
};

class processor_queue {
public:

  int empty() { assert_interrupts_disabled(); return sz == 0; }
  int size() { assert_interrupts_disabled(); return sz; }
  void pop();
  cpu* front() { assert_interrupts_disabled(); return frnt; }
  void push(cpu* cpu_ptr);

private:
  cpu* frnt;
  cpu* bck;
  int sz;
};

#endif
