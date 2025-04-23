#include "join_threads.h"

join_threads::join_threads(std::vector<std::thread>& _threads) : threads(_threads) {}

//destructor lopps threads and joins them if they are in a joinable state
join_threads::~join_threads()
{
	for (unsigned long i = 0; i < threads.size(); i++)
	{
		//checks to see if the thread is running
		if (threads[i].joinable())
			threads[i].join(); //if so wait for thread to finish
	}
}