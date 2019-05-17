#include<fiber_control.h>
#include<iostream>
Tls Fiber_Control::tls;
void init_context(ucontext_t * uc)
{
	getcontext(uc);
	uc->uc_link=0;	  
  	uc->uc_stack.ss_sp = (char *) malloc(STACK_SIZE);
    uc->uc_stack.ss_size = STACK_SIZE;
    uc->uc_stack.ss_flags = 0;
}
struct trampoline_args {
  std::function<void(void)> fn;
};
Fiber_Control::Fiber_Control()
:stop(false),waiting(true)
{	
	tls.parent = this;
	tls.cur = NULL;
	tls.pre = NULL;
	tls.garbage = NULL;
	ucontext_t * cp = &tls.base;
	init_context(cp);
	auto thread_init = std::bind(&Fiber_Control::init, this);
	thread.start(thread_init);
}
void Fiber_Control::init()
{	
	
	waiting=true;
	mutex.lock();
	while(not stop)
	{
		Fiber* f = get_fiber();		
		if (f != NULL)
		{
			mutex.unlock();
			waiting=false;
			yield_to(f);
			waiting=true;
			mutex.lock();
		}
		else
		{
			cond.wait(mutex);		
		}		
	}
	mutex.unlock();

}
Fiber_Control& Fiber_Control::get_instance()
{
	static Fiber_Control singleton;
	return singleton;
}
Fiber* Fiber_Control::launch(std::function<void()> fn)
{
	Fiber* f = new Fiber;
	f->terminate=false;
	f->inited=false;
	f->parent=this;
	f->fn=fn;
	workerqueue.push(f);
	if(waiting == true)
	{
		mutex.lock();
		cond.signal();
		mutex.unlock();
	}
	return f;
	
}
Fiber* Fiber_Control::get_fiber()
{
	if ( workerqueue.empty() )
	{
		return NULL;
	}
	else
	{
		Fiber* fp = workerqueue.front();
		workerqueue.pop();		
		return  fp;
	}
}

void Fiber_Control::join()
{
	thread.join();
}
void Fiber_Control::yield_to(Fiber* next_fib)
{
	if ( next_fib != NULL )
	{

		tls.pre = tls.cur;
		tls.cur = next_fib;
		if( tls.pre != NULL )
		{	
			// std::cout<<"run to here Fiber_Control:98"<<std::endl;				

			if (tls.cur->inited == false)
			{
				trampoline_args* args = new trampoline_args;	
				args->fn=tls.cur->fn;
				init_context(&(tls.cur->context));
				tls.cur->context.uc_link=&(tls.pre->context);
				makecontext(&(tls.cur->context), (void (*)())trampoline, 1, args);
				tls.cur->inited = true;
			}

			swapcontext(&(tls.pre->context), &(tls.cur->context));
			//setcontext(&(tls.cur->context));
		}
		else//tls.pre == null
		{	
			
			// trampoline_args* args = new trampoline_args;
			// args->fn=tls.cur->fn;
			// init_context(&(tls.cur->context));

			// init_context(&(tls.base));
			// tls.cur->context.uc_link=&(tls.base);
			// makecontext(&(tls.cur->context), (void (*)())trampoline, 1, args);
			if (tls.cur->inited == false)
			{
				trampoline_args* args = new trampoline_args;	
				args->fn=tls.cur->fn;
				init_context(&(tls.cur->context));
				tls.cur->context.uc_link=&(tls.pre->context);
				makecontext(&(tls.cur->context), (void (*)())trampoline, 1, args);
				tls.cur->inited = true;
			}
			swapcontext(&(tls.base), &(tls.cur->context));
		}
	}
	else
	{
		// std::cout<<"run to here: Fiber_Control:122"<<std::endl;
		if(tls.cur && tls.cur->terminate)
		{
			tls.pre=tls.cur;
			tls.cur=NULL;
			setcontext(&(tls.base));
		}
		else
			return;
	}
	if( tls.pre )
	{
		// std::cout<<"runt to here fiber_control:134"<<std::endl;
		reschedule_fiber(tls.pre);
	}
	tls.pre = NULL;
}


// the trampoline to call the user function. This function never returns
void Fiber_Control::trampoline(void* _args) {
//void trampoline(int _args) {
  // we may have launched to here by switching in from another fiber.
  // we will need to clean up the previous fiber
  
  if (tls.pre) 
  	tls.parent->reschedule_fiber(tls.pre);
  tls.pre = NULL;
  std::cout<<" call trampoline\n";
  trampoline_args* args = reinterpret_cast<trampoline_args*>(_args);
  try {
    args->fn();
  } catch (...) {
  }
  delete args;
  exit();
}

void Fiber_Control::reschedule_fiber(Fiber* f)
{
	if (not f->terminate)
	{
		workerqueue.push(f);	
	}	
}

void Fiber_Control::exit()
{
	tls.cur->terminate=true;
	yield();

}
void Fiber_Control::yield()
{

	Fiber * next_fib=tls.parent->get_fiber();
	tls.parent->yield_to(next_fib);
}