#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>
#include <utility>

#include "function_wrapper.h"


//modeled on std::queue<>
//The interface is different from the standard queue because of the 
//constraints of writing a data structure thats safe for concurrent access
template<typename T>
class threadsafe_queue
{
private:
	//mutable is a storage class specifier that allows a specific data member of a class to be modified 
	//even if its part of an object declared as const
	mutable std::mutex mut;
	std::queue<T> data_queue;
	std::condition_variable data_condition;

public:
	threadsafe_queue();
	void push(T new_value);
	void wait_and_pop(T& value);
	std::shared_ptr<T> wait_and_pop();
	bool try_pop(T& value);
	std::shared_ptr<T> try_pop();
	bool empty() const;
};