#pragma once
#include <vector>
#include <thread>

//purpose of this class is to wait for all running threads to complete when the thread pool is destroyed
//this allows for values to be returned and for all threads to be joined regardless of how the function exits

class join_threads
{
	std::vector<std::thread>& threads;

public:
	explicit join_threads(std::vector<std::thread>& _threads);
	~join_threads();
};