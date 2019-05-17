#include<fiber_group.h>
#include<iostream>
void Fiber_group::invoke(std::function<void()> fn)
{
	fn();
	--funs_running;
	if (funs_running == 0)
	{
		 mutex.lock();
		 cond.signal();
		 mutex.unlock();
	}
}
void Fiber_group::launch(std::function<void()> fn)
{	
	++funs_running;

	auto fnn=std::bind(&Fiber_group::invoke, this, fn);
	Fiber_Control::get_instance().launch(fnn);
	// Fiber_Control& hhh= Fiber_control::get_instance();
	// std::cout<<&hhh<<std::endl;
	//hhh.launch(fn);
}

void Fiber_group::join(void)
{
	mutex.lock();
	while( funs_running != 0)
	{
		cond.wait(mutex);
	}
	mutex.unlock();
	//Fiber_Control::get_instance().join();

	// Fiber_Control& hhh= Fiber_control::get_instance();
	// hhh.
}