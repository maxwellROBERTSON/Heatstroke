#include "function_wrapper.h"

//operator overload on the () operator, this executes the function wrapped by this class
void function_wrapper::operator()()
{
	implicit->call();
}

//move constrctor from Rvalue
function_wrapper::function_wrapper(function_wrapper&& other) :
	implicit(std::move(other.implicit))
{}

//assignment operator to move one function_wrapper object into another
function_wrapper& function_wrapper::operator=(function_wrapper&& other) noexcept
{
	implicit = std::move(other.implicit);
	return *this;
}