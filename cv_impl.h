#ifndef CV_IMPL_H
#define CV_IMPL_H

#include "cv.h"
#include "context_wrapper.h"
#include <list>
#include <memory>

class cv::impl {

	friend cv;
	
	public:
		impl()
		{ }
	
	private:
    std::list<context_wrapper*> cvQ;

};

#endif
