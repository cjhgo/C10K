#include <pthread_tools.h>

void Thread::start( std::function<void(void)> spawn_routine)
{
	pthread_create(&thread_t, NULL, invoke, (void *) (new invoke_args(spawn_routine)) );
}

void Thread::join()
{
	pthread_join(thread_t, NULL);
}

void* Thread::invoke(void* _args)
{
	Thread::invoke_args* args = (Thread::invoke_args*) _args;
	args->spawn_routine();
}