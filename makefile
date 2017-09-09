#EECS 482 p1 MakeFile
SHELL := /bin/bash
#to run with testcase run: make VAR="testcase file" all
#Variable for flags
CPP_FLAGS = -Wall -Wextra -Werror -std=c++11 -pthread -ldl

#name of executable
EXEC_NAME = p2

#compiler
CC = g++
#cc files
FILES = $(wildcard *.cc)
#object files
OBJ = context_queue.o global.o cpu.o thread.o cv.o mutex.o context_wrapper.o 

all: p2
debug: CPP_FLAGS += -g
debug: p2
object: $(OBJ)

p2:  
	$(CC) ${VAR} $(CPP_FLAGS) $(FILES) libcpu.a -o ${EXEC}

context_queue.o: context_queue.h
	$(CC) -c $(CPP_FLAGS) context_queue.cc
	
global.o: global.h context_wrapper.h context_queue.h
	$(CC) -c $(CPP_FLAGS) global.cc

thread.o: cv.h mutex.h cpu.h context_wrapper.h global.h cpu_impl.h thread_impl.h
	$(CC) -c $(CPP_FLAGS) thread.cc

cpu.o: cpu.h thread.h context_wrapper.h global.h cpu_impl.h cv_impl.h
	$(CC) -c $(CPP_FLAGS) cpu.cc 

mutex.o: mutex.h global.h thread.h mutex_impl.h
	$(CC) -c $(CPP_FLAGS) mutex.cc

cv.o: cv.h global.h thread.h cv_impl.h
	$(CC) -c $(CPP_FLAGS) cv.cc

context_wrapper.o: context_wrapper.h thread.h global.h thread_impl.h
	$(CC) -c $(CPP_FLAGS) context_wrapper.cc

clean: 
	rm $(EXEC_NAME) $(OBJ) *~ *.gch


