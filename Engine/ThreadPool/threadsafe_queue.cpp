#include "threadsafe_queue.h"


template<typename T>
threadsafe_queue<T>::threadsafe_queue()
{}

template class threadsafe_queue<function_wrapper>;


template<typename T>
void threadsafe_queue<T>::push(T new_value)
{
	//locks the mutex passed in, std::mutex is placed in angled brackets
	//since std::lock_guard is a template class, it tells lock_guard
	//what type of object it will be locking
	std::lock_guard<std::mutex> lk(mut);

	//critical zone 
	data_queue.push(std::move(new_value));


	//notify waiting threads that the current thread has finished with the data
	data_condition.notify_one();
}

template<typename T>
void threadsafe_queue<T>::wait_and_pop(T& value)
{
	//lock mutex
	std::unique_lock<std::mutex> lk(mut);

	data_condition.wait(lk, [this] {return !data_queue.empty(); });
	value = std::move(data_queue.front());
	data_queue.pop();
}

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop()
{
	//lock mutex
	std::unique_lock<std::mutex> lk(mut);

	//check condition variable and wait, this automatically releases the mutex
	//when the condition variable is notified by a thread currently working on the data, this thread will wake up and lock the mutex,
	//check the condition variable again in this case if the data_queue isn't empty it will then continue execution in this function
	data_condition.wait(lk, [this] {return !data_queue.empty(); });

	//create shared pointer to the element at the front of the queue
	std::shared_ptr<T> res(
		std::make_shared<T>(std::move(data_queue.front())));

	//remove element from front of the queue
	data_queue.pop();

	//return shared pointer
	return res;
}

//tries to take a task off the queue, if nothing is on the queue false is returned
//if there is something on the queue the true is returned and value is set to the task taken off the queue
template<typename T>
bool threadsafe_queue<T>::try_pop(T& value)
{
	std::lock_guard<std::mutex> lk(mut);
	if (data_queue.empty()) return false;

	//data_queue.front() accesses first item of queue and returns ref to it
	value = std::move(data_queue.front());

	//variable 'value' has been assigned the front of the queue so the 
	//first element can now be removed
	data_queue.pop();

	//return success flag
	return true;
}

template<typename T>
std::shared_ptr<T> threadsafe_queue<T>::try_pop()
{
	std::lock_guard<std::mutex> lk(mut);
	if (data_queue.empty()) return std::shared_ptr<T>();

	std::shared_ptr<T> res(
		std::make_shared<T>(std::move(data_queue.front())));
	data_queue.pop();
	return res;
}

template<typename T>
bool threadsafe_queue<T>::empty() const
{
	//lock mutex
	std::lock_guard<std::mutex> lk(mut);
	//access data queue and return empty
	return data_queue.empty();
}