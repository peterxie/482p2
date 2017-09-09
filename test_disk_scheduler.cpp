#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <queue>
#include <map>
#include <climits>
#include <cmath>
#include "thread.h"

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::vector;
using std::queue;
using std::map;

mutex mutex1;
int num_request_threads;
int queue_cap;
int max_disk_queue;
cv servicer_waiting;
cv requester_waiting;
vector<queue<int> > requests;
map<int,int> scheduler;

void parentThread(void *);
void serviceThread(void *);
void requestThread(void *);

//Need to mutex in parentThread for queue_cap, reading num_request_threads?

int main() {


	num_request_threads = 5;
	max_disk_queue = 3;
	queue_cap = (num_request_threads >= max_disk_queue) ? max_disk_queue : num_request_threads;

	queue<int> q1, q2, q3, q4, q5;
	q1.push(785); q1.push(53);
	q2.push(350); q2.push(914);
	q3.push(827); q3.push(567);
	q4.push(302); q4.push(230);
	q5.push(631); q5.push(11);
	
	requests.push_back(q1);
	requests.push_back(q2);
	requests.push_back(q3);
	requests.push_back(q4);
	requests.push_back(q5);
	
	//cpu::boot((thread_startfunc_t) parentThread, nullptr, 0);
	cpu::boot(1, (thread_startfunc_t) parentThread, (void *) "parent thread", false, false, 0);
	
	return 0;
}

void parentThread(void *a) {

	int requester_id = 0;
	
	char *id = (char *) a;
	cout << id << " running" << endl;

	//Spawn service thread
	thread((thread_startfunc_t) serviceThread, (void *) "service thread");

	//Spawn requester threads
	for(int i = 0; i < num_request_threads; ++i) {
		thread((thread_startfunc_t) requestThread, (void *) (intptr_t) requester_id);
		++requester_id;
	}

}

void serviceThread(void *a) {

	char *id = (char *) a;
	cout << id << " running" << endl;
	
	int current_track = 0;

	mutex1.lock();

	while(num_request_threads) {

		//queue_cap = (num_request_threads >= max_disk_queue) ? max_disk_queue : num_request_threads;

		//If scheduler not at full capacity, wait
		while((int) scheduler.size() < queue_cap) {
			servicer_waiting.wait(mutex1); 
		}

		int min_track_dist = INT_MAX;
		int requester, track;

		//Choose requester to service
		for(auto req : scheduler) {
			int track_dist = abs(req.second - current_track);
	
			if(track_dist < min_track_dist) {
				min_track_dist = track_dist;
				requester = req.first;
				track = req.second;
			}
		}

		current_track = track;
		cout << "service requester " << requester << " track " << current_track << endl;
		scheduler.erase(requester);
		requests[requester].pop();

		if(requests[requester].empty()) {
			--num_request_threads;
			queue_cap = (num_request_threads >= max_disk_queue) ? max_disk_queue : num_request_threads;			
		}

		requester_waiting.broadcast();	
	}

	mutex1.unlock();

}

void requestThread(void *a) {

	int requester = (intptr_t) a;

	mutex1.lock();	

	while(!requests[requester].empty()) {

		//Check if queue is already full
		while(queue_cap == (int) scheduler.size()) {
			requester_waiting.wait(mutex1);
		}

		//If not, request service
		int track = requests[requester].front();

		cout << "requester " << requester << " track " << track << endl;
		scheduler[requester] = track;

		servicer_waiting.signal();

		//Check if it already has a request being processed by looking
		//through map of scheduled requests
		while(scheduler.find(requester) != scheduler.end()) {
			requester_waiting.wait(mutex1);
		}

	}

	mutex1.unlock();
}

