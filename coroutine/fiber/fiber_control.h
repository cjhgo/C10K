#ifndef MYFIBER_FIBER_CONTROL
#define MYFIBER_FIBER_CONTROL
#include<functional>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include<queue>
#include<pthread_tools.h>
#define STACK_SIZE 16384  
class Fiber_Control;
struct Fiber
{
	Fiber_Control* parent;
	std::function<void()> fn;
	ucontext_t context;
	bool terminate;
	bool inited;
};
struct Tls
{
	Fiber_Control* parent;
	Fiber* cur;
	Fiber* pre;
	Fiber* garbage;
	ucontext_t base;

};
class Fiber_Control
{
public:
	//static Fiber_Control singleton;
	static Fiber_Control& get_instance();
	void init();
	Fiber* launch(std::function<void()> fn);
	Fiber_Control();
	void join();
	static void exit();
	static void yield();
	static void trampoline(void* _args);
	static Tls tls;
private:	
	Thread thread;
	Mutex mutex;
	Conditional cond;
	bool waiting;
	bool stop;
	std::queue<Fiber*> workerqueue;
	Fiber* get_fiber();	
	void yield_to(Fiber* next_fib);
	void reschedule_fiber(Fiber* f);
};

void init_context(ucontext_t * uc);
#endif