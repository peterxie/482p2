#ifndef MUTEX_IMPL_H_
#define MUTEX_IMPL_H_

#include "mutex.h"
#include "context_wrapper.h"
#include <list>
#include <memory>

class mutex::impl {

	friend mutex;
	friend cv;

	public:
		impl()
		: free(true)
		{ }

	private:

    void unlock_helper();
    void lock_helper();

		bool free;
    std::list<context_wrapper*> mutexQ;
    context_wrapper* owner;
};



#endif
