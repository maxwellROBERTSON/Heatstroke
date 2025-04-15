#pragma once
#include <memory>

//This class wraps functions which are passed to the queue
//this is use of the type-erasure pattern. 


class function_wrapper
{
	struct implicit_base
	{
		//defines a pure virtual function which must be implemented by the derived class
		virtual void call() = 0;
		virtual ~implicit_base() {}
	};

	//A unique pointer of type implicit base
	std::unique_ptr<implicit_base> implicit;

	template<typename F>
	struct implicit_type : implicit_base
	{
		//Member which is assigned in the constructor
		F f;

		//constructor which will take an object of type F and assign it to member f
		implicit_type(F&& _f) : f(std::move(_f)) {}

		//virtual function from inherted implicit_base is implemented here
		void call() 
		{ 
			//wrapped function is called here
			f(); 
		}
	};

public:
	//this constructor takes an Rvalue reference and moves its data into member named 'implict' leaving 'f' in an invalid state
	template <typename F>
	function_wrapper(F&& f) : implicit(new implicit_type<F>(std::move(f)))
	{}

	//operator overload on the () operator, this overload takes no arguments and returns void
	void operator()();

	//means that the copiler will create a default constructor for this class
	function_wrapper() = default;

	//move constructor which takes an Rvalue 
	function_wrapper(function_wrapper&& other);

	//assignment operator to move one function_wrapper object into another, takes an Rvalue 
	function_wrapper& operator=(function_wrapper&& other) noexcept;

	//These are deleted to prevent copy operations
	//delete the copy constructor to copy const objects
	function_wrapper(const function_wrapper&) = delete;
	//delete the copy constructor to copy non-const objects
	function_wrapper(function_wrapper&) = delete;
	//delete the assignemnet operator
	function_wrapper& operator=(const function_wrapper&) = delete;
};