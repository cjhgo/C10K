#ifndef MYFIBER_FIBER_GROUP
#define MYFIBER_FIBER_GROUP
#include<fiber_control.h>
#include<pthread_tools.h>
class Fiber_group
{
public:
	Fiber_group():funs_running(0){};
	~Fiber_group(){};
	void launch(std::function<void()> fn);
	void join();
private:
	void invoke(std::function<void()> fn);	
	Mutex mutex;
	Conditional cond;
	size_t funs_running; 	

};
#endif