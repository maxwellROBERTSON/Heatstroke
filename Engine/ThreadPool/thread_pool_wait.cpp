#include "thread_pool_wait.h"

thread_pool_wait* thread_pool_wait::instance = new thread_pool_wait();

//function to get the single instance of the thread pool class
thread_pool_wait* thread_pool_wait::get_instance()
{
	//if the instance doesn't exist
	if (thread_pool_wait::instance == nullptr)
	{
		//create it
		thread_pool_wait::thread_pool_wait();
	}

	//return a pointer to the static instance
	return thread_pool_wait::instance;
}


//constructor
thread_pool_wait::thread_pool_wait() : done(false), joiner(threads)
{
	//get the number of threads that the system can support
	unsigned const thread_count = std::thread::hardware_concurrency();


	std::cout << "Creating thread pool... \n"
		"Threads avaiable: " << thread_count << std::endl;

	try
	{
		for (int i = 0; i < thread_count; i++)
		{
			//each thread will run the worker_thread function
			//create thread, new thread calls worker_thread() from this class, push new thread to threads vector 
			threads.push_back(std::thread(&thread_pool_wait::worker_thread, this));
		}
	}
	catch (...)
	{
		done = true;
		throw;
	}
}

//here tasks (functions) are executed
void thread_pool_wait::worker_thread()
{
	//each thread will loop and attempt to take tasks off the queue
	while (done == false)
	{
		//function wrapper for void argumentless functions, this is the object functions from the queue will be assigned to
		function_wrapper task;

		//get work from the queue
		work_queue.wait_and_pop(task); //thread sleeps while waiting

		std::thread::id this_id = std::this_thread::get_id();
		std::cout << "#: " << this_id << std::endl;

		//when wait_and_pop returns with a task
		task(); //execute task
	}
}


//destructor
thread_pool_wait::~thread_pool_wait()
{
	std::cout << "Thread Pool Destroyed" << std::endl;
	done = true;
}



void thread_pool_wait::run_pending_task()
{
	//function wrapper object which will be assigned a task from the queue
	function_wrapper task;

	//see if theres anything on the queue, pop it if there is
	if (work_queue.try_pop(task))
	{
		//execute task from queue
		task();
	}
}