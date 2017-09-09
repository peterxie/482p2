#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>

using std::min;
using std::lower_bound;

using std::cout;
using std::endl;

using std::abs;

/*************** diskQueue.h **********/
// Request data that is sent to the disk queue.
struct Request {
	int requester;
	int track;

	Request(int requester_, int track_)
	: requester{requester_}, track{track_} {}

	Request(int track_)
	: requester{-1}, track{track_} {}
	
	bool operator<(const Request &r) const
	{
		return track < r.track;
	}
};

struct DiskQueue {
	void initialize(int max_disk_queue_, int live_requesters_);

	void add_request(Request r);

	Request service_request();
	
	bool full() // note, zero == zero implies it is full.
	{return queue.size() == static_cast<unsigned int>(max_disk_queue);}
	
	bool finished()
	{return live_requesters == 0;}
	
	void thread_done() 
	{max_disk_queue = std::min(max_disk_queue, --live_requesters);}

	// members
	std::vector<Request> queue;
	int max_disk_queue;
	int live_requesters;
	int cur_track;
};



void DiskQueue::initialize(int max_disk_queue_, int live_requesters_)
{
	max_disk_queue = min(max_disk_queue_, live_requesters_);
	live_requesters = live_requesters_;
	cur_track = 0;
}

void DiskQueue::add_request(Request r)
{
	cout << "requester " << r.requester << " track " << r.track << endl;
	// Keep queue ordered by track number.
	auto it = lower_bound(queue.begin(), queue.end(), r);
	if(it == queue.end())
		queue.push_back(r);
	else
		queue.insert(it, r);
}

Request DiskQueue::service_request()
{
	// Get an iterator to the request with the closest track.
	auto it = lower_bound(queue.begin(), queue.end(), cur_track);
	if(it == queue.end())
		--it;
	else if(it != queue.begin()) {
		int u_distance = abs(it->track - cur_track);
		int d_distance = abs((it-1)->track - cur_track);
		if(d_distance < u_distance)
			--it;
	}

	// Service and remove this request.
	cout << "service requester " << it->requester << " track " << it->track << endl;
	cur_track = it->track;
	Request req = *it;
	queue.erase(it);
	return req;
}
/****************************************/
















#include "thread.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <memory>
#include <mutex>
#include <utility>

using std::cerr;

using std::ifstream;

using std::vector;

using std::istream_iterator;
using std::back_inserter;

using std::stoi;

using std::copy;

using std::unique_ptr;

using std::lock_guard;

using std::pair;
using std::make_pair;

struct RequestStarter {
	int requester;
	//char* filename;
};

struct MainArgs {
	int num_threads_;
	int max_disk_queue_;
	char** filenames;
};

void run(void* mainArgs);
void requester_thread(void* requestStarter);
void servicer_thread(void* );


// Global disk queue
DiskQueue g_diskQueue;
// Monitoring variabls
mutex g_mutex;
cv g_waiting_to_request;
cv g_waiting_to_service;
vector<pair<unique_ptr<cv>, bool>> g_waiting_for_service;

vector<vector<int>> queues;


int main()
{

	
	MainArgs mainArgs;
	mainArgs.num_threads_ = 5;
	mainArgs.max_disk_queue_ = 3;
	//mainArgs.filenames = &(argv[2]);

	vector<int> q0{785, 53}; queues.push_back(q0);
	vector<int> q1{350, 914}; queues.push_back(q1);
	vector<int> q2{827, 567}; queues.push_back(q2);
	vector<int> q3{302, 230}; queues.push_back(q3);
	vector<int> q4{631, 11}; queues.push_back(q4);


	//cpu::boot((thread_startfunc_t) run, static_cast<void *>(&mainArgs), 0);
	cpu::boot(1, (thread_startfunc_t) run, static_cast<void *>(&mainArgs), false, false, 0);
		
 	return 0;
}



void run(void* mainArgs)
{
	MainArgs* args = (MainArgs*) mainArgs;
	
	// create disk queue
	g_diskQueue.initialize(args->max_disk_queue_, args->num_threads_);
	
	// Run request threads
	for(int i = 0; i < args->num_threads_; ++i) {
		// Create cv for when this thread is waiting to be serviced
		g_waiting_for_service.push_back(make_pair(unique_ptr<cv>(new cv), false));
		// thread: i, input file: args->filenames[i]]
		RequestStarter* rs_ptr{new RequestStarter{i}};
		thread t((thread_startfunc_t)requester_thread, static_cast<void*>(rs_ptr));
	}

	// Run Servicer thread.
	thread servicer((thread_startfunc_t)servicer_thread, nullptr);
}

void requester_thread(void* requestStarter)
{
	RequestStarter* rs_ptr =  static_cast<RequestStarter*>(requestStarter);
	
	vector<int> tracks = queues[rs_ptr->requester];
	//ifstream input{rs_ptr->filename};
	//istream_iterator<int> input_iter{input};
	// Fill 'tracks' with the track numbers to be requested.
	//copy(input_iter, istream_iterator<int>(), back_inserter(tracks));

	// Get each track from file.
	lock_guard<mutex> lg{g_mutex};
	for(int track : tracks) {
		while(g_diskQueue.full()) {
			g_waiting_to_request.wait(g_mutex);
		}

		// Add new request to the disk queue
		g_diskQueue.add_request({rs_ptr->requester, track});
		// Now waiting for service
		g_waiting_for_service[rs_ptr->requester].second = true;
				
		// wait for request to be filled
		while(g_waiting_for_service[rs_ptr->requester].second == true) {
			g_waiting_to_service.signal();
			g_waiting_for_service[rs_ptr->requester].first->wait(g_mutex);
		}
	}
	
	// Notify the diskQueue that we are done.
	g_diskQueue.thread_done();
	if(g_diskQueue.full())
		// Queue wasn't full until we --num_threads
		g_waiting_to_service.signal();
			
	delete rs_ptr;
}

void servicer_thread(void* )
{
	lock_guard<mutex> lg{g_mutex};
	while(true) {
		while(!g_diskQueue.full()) {
			g_waiting_to_service.wait(g_mutex);
		}
		
		// break out of loop when there are no threads left.
		if(g_diskQueue.finished())
			break;

		// Service a request
		Request req = g_diskQueue.service_request();
		
		g_waiting_to_request.signal();
		g_waiting_for_service[req.requester].second = false;
		g_waiting_for_service[req.requester].first->signal();
	}
}


