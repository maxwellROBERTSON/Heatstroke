#pragma once
#include <atomic>
#include <thread>
#include <vector>
#include <functional>
#include <iostream>
#include <future>
#include <type_traits>

#include "threadsafe_queue.h"
#include "join_threads.h"
#include "function_wrapper.h"



class thread_pool_wait
{
	std::atomic_bool done;

	//the queue where the works is sent to which threads take from
	threadsafe_queue<function_wrapper> work_queue;

	//the threads our pool has
	std::vector<std::thread> threads;

	//class which waits for threads to finish in the case of an exception
	//being thrown, deals with clean-up
	join_threads joiner;


	//here tasks (functions) are executed
	void worker_thread();

public:
	//constructor
	thread_pool_wait();

	//destructor
	~thread_pool_wait();

	//submit free function
	//submit() puts tasks on the queue and returns a handle to that task to allow the main thread to wait for its completion
	template<typename FunctionType>
	std::future<typename std::invoke_result<FunctionType>::type> submit(FunctionType f)
	{
		//defines the return type of the callable (FUnctionType) passed into submit
		typedef typename std::invoke_result<FunctionType>::type result_type;

		//create a packaged task returns a type result_type, the () mean that 
		//the callable (the task) doesn't take any arguments
		std::packaged_task<result_type()> task(std::move(f));

		//create a future of the type we deduced, the value of the future is the result of the packaged_task 'task'
		//we call get_future which returns the future from the packaged_task, this can only be done once for each
		//packaged task. This future object will be returned from this function and can have its get() called later to block execution
		//and return the result of 'task'
		std::future<result_type> result(task.get_future());

		//add task to the queue
		work_queue.push(std::move(task));

		//return the std::future
		return result;
	}


	//submit member function (function, object)
	template<typename FunctionType, typename Object>
	auto submit(FunctionType f, Object obj)
	{
		//bind the object and its member function 
		auto function = std::bind(f, obj);

		//defines the return type of the callable (FUnctionType) passed into submit
		typedef typename std::invoke_result<decltype(function)>::type result_type;

		//create a packaged task returns a type result_type, the () mean that 
		//the callable (the task) doesn't take any arguments
		std::packaged_task<result_type()> task(std::move(function));

		//create a future of the type we deduced, the value of the future is the result of the packaged_task 'task'
		//we call get_future which returns the future from the packaged_task, this can only be done once for each
		//packaged task. This future object will be returned from this function and can have its get() called later to block execution
		//and return the result of 'task'
		std::future<result_type> result(task.get_future());

		//add task to the queue
		work_queue.push(std::move(task));

		//return the std::future
		return result;
	}

	void run_pending_task();
};