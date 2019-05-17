#ifndef MYFIBER_PTHREAD_TOOLS
#define MYFIBER_PTHREAD_TOOLS
#include <functional>
#include <pthread.h>


class Mutex
{
public:
	pthread_mutex_t mutex_t;
	Mutex()
	{
		pthread_mutex_init(&mutex_t, NULL);
	};
	~Mutex()
	{
		pthread_mutex_destroy(&mutex_t);
	};
	void lock()
	{
		pthread_mutex_lock(&mutex_t);
	}
	void unlock()
	{
		pthread_mutex_unlock(&mutex_t);
	}
};
class Conditional
{
private:
	pthread_cond_t cond_t;
public:	
	Conditional()
	{
		pthread_cond_init(&cond_t, NULL);
	}
	~Conditional()
	{
		pthread_cond_destroy(&cond_t);
	}
	void wait(Mutex& mut) 
	{
		pthread_cond_wait(&cond_t, &mut.mutex_t);
	}
	void signal() 
	{
		pthread_cond_signal(&cond_t);
	}

};

class Thread
{
public:
	Thread(){};
	~Thread(){};
	void start(std::function<void(void)> spawn_routine);
	void join();
private:
	pthread_t thread_t;
	struct invoke_args{
      std::function<void(void)> spawn_routine;
      invoke_args(const std::function<void(void)>& spawn_routine):
      	spawn_routine(spawn_routine){};
    };
	static void* invoke(void* _args);
};
#endif
