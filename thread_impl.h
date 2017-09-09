#include "thread.h"
#include "cpu.h"
#include "global.h"
#include <ucontext.h>
#include <memory>
#include "cpu_impl.h"

//#include <cassert>

//#define NDEBUG


class thread::impl {
  public:
    
    context_wrapper* ucp_weak;
    impl(context_wrapper* context)
      :ucp_weak(context)
    {
    }

    ~impl(){ ucp_weak = nullptr; }
};
