#include <iostream>
#include "scheduler.hpp"
#include "userspace_scheduler.hpp"


namespace scheduler
{
  
namespace tools
{
  // Porównanie potrzebne podczas budowania kolejki priorytetowej
  // złożonej z ULS-ów. nie warto wspominać gdzieś tam wyżej.
  class uls_comp
  {
    public:
      bool operator() ( userspace_scheduler* u1, userspace_scheduler* u2 )
      {
        return ( u1->get_workload() >= u2->get_workload() );
      }
  };
  
}
  
}

using namespace scheduler::tools;

scheduler::ueber_scheduler::ueber_scheduler()
{
}

void* scheduler::ueber_scheduler::go(void* obj)
{
  ueber_scheduler* sched_obj = ( ueber_scheduler* )obj;
  pthread_barrier_wait( &sched_obj->barrier );
  //std::cout << "Pthread " << pthread_self() << " poszedł" << std::endl; 
  
  sched_obj->run();
  //sched_obj->start();
  return 0;
}

void 
scheduler::ueber_scheduler::init( std::list< userspace_scheduler* >* local_schedulers )
{
  base_coroutine = libcoro::factory::create_coroutine();
  libcoro::factory::assign_coroutine( this );
  //manager.init();
  
  scheduler_end = false;
  
  // pthreads start
  
  pthread_barrier_init( &barrier, 0, 2 );
  
  pthread_mutex_init( &container_is_ready, 0 );
  
  pthread_attr_init (&stack_attr);
  pthread_create( &main_thread, &stack_attr, &ueber_scheduler::go, (void*)this );
  
  // pthreads end
  
  if ( local_schedulers == 0 )
  {
      create_local_schedulers();
  }
  else
  {
      create_local_schedulers( local_schedulers );
  }
  blocked_num = 0;
}

void
scheduler::ueber_scheduler::create_local_schedulers( std::list< userspace_scheduler* >* list_ )
{
    //Utwórz tyle planistów ULT, ile jest dostępnych rdzeni dla procesu:
    for ( std::list< userspace_scheduler* >::iterator i = list_->begin();
          i != list_->end();
          i++ )
    {
        userspace_scheduler::ptr s = *i ;

        // Dla każdego userspace scheduler-a utwórz dwie rurki:
        // 1. Do zapisu (in): zapisuje ueber_scheduler, odczytuje userspace_scheduler
        // 2. Do odczytu (out)
        raw_pipe* srp = new raw_pipe();
        srp->init();
        pipes.push_back(srp);

        s->init(srp);
        schedulers.push_back( s );
    }
}

void
scheduler::ueber_scheduler::create_local_schedulers()
{
  //Utwórz tyle planistów ULT, ile jest dostępnych rdzeni dla procesu:
  cpu_set_t cs;
  int rv = sched_getaffinity(0, sizeof(cs), &cs);
  if ( rv == 0 )
  {
    for ( int cores=0; cores < ( CPU_COUNT( &cs ) ); cores++ )
    {
      userspace_scheduler::ptr s = new userspace_scheduler( this );
      
      // Dla każdego userspace scheduler-a utwórz dwie rurki:
      // 1. Do zapisu (in): zapisuje ueber_scheduler, odczytuje userspace_scheduler
      // 2. Do odczytu (out)
      raw_pipe* srp = new raw_pipe();
      srp->init();
      pipes.push_back(srp);
      
      s->init(srp);
      schedulers.push_back( s );
    }
  }
}

bool 
scheduler::ueber_scheduler::finished()
{
  pthread_mutex_lock( &finish_check_lock );
  
  bool retval = scheduler_end;
  
  pthread_mutex_unlock( &finish_check_lock );
  
  return retval;
}

void 
scheduler::ueber_scheduler::finish()
{
  pthread_mutex_lock( &finish_check_lock );
  
  scheduler_end = true;
  
  pthread_mutex_unlock( &finish_check_lock );
}

void 
scheduler::ueber_scheduler::run()
{
  int total_workload;
  userspace_scheduler::list::iterator it;
  std::list<raw_pipe*>::iterator pipe_it;
  
  do
  {
    for ( pipe_it = pipes.begin();
    pipe_it != pipes.end();
    pipe_it++ )
    {
      spawned_data pc;
      pc.d = NOTHING;
      while ( read_and_interpret( *pipe_it, pc ) )
      {
        switch ( pc.d )
        {
          case SPAWN_CONFIRMED:
            blocked_num--;
            break;
            
          case FIBER_SPECIFIC:
						send( pc );
            break;
            
          case BLOCK:
            break;
            
          default:
            break;
        }
				pc.d = NOTHING;
      }
    }
    
    total_workload = 0;
    for ( it = schedulers.begin();
    it != schedulers.end();
    it++
    )
    {
      total_workload += (*it)->get_workload();
    }

		// nie mam wątków do uruchomienia - pora kończyć.
		if ( ( total_workload == 0 ) && ( blocked_num == 0 ) )
		{
			// poinformuj wszystkie worker schedulery o końcu pracy
			spawned_data end_info;
			end_info.d = END;
			send( end_info );
		}
  } while ( (total_workload != 0) || (blocked_num != 0) ) ;

	bool schedulers_have_finished_work;
	do
	{
		schedulers_have_finished_work = true;
		for ( it = schedulers.begin();
				it != schedulers.end();
				it++
				)
		{
			schedulers_have_finished_work &= (*it)->ended();
		}
	}
	while ( ! schedulers_have_finished_work )
		;
	
  //std::cout << "total workload: " << total_workload << ", blocked num: " << blocked_num << std::endl;
}

bool 
scheduler::ueber_scheduler::get_from_pipe(scheduler::raw_pipe* rp, scheduler::spawned_data& sp)
{
    return rp->read_out( sp );
}

bool 
scheduler::ueber_scheduler::read_and_interpret(scheduler::raw_pipe* rp, scheduler::spawned_data& pc)
{
  return get_from_pipe( rp, pc );
}

void 
scheduler::ueber_scheduler::start()
{
  ::pthread_attr_t stub_attr;
  ::pthread_t stub_thread;
  
  ::pthread_attr_init ( &stub_attr );
  ::pthread_create( &stub_thread, &stub_attr, &ueber_scheduler::stub_go, ( void* )this );
}

bool 
scheduler::ueber_scheduler::empty()
{
  return ready.empty();
}

void 
scheduler::ueber_scheduler::join_u_sch()
{
  start();
  pthread_join( main_thread, 0 );
}

libmanager::manager::ptr scheduler::ueber_scheduler::get_manager()
{
  return &manager;
}

void scheduler::ueber_scheduler::move_to_blocked(fiber::fiber::ptr )
{
}

scheduler::ueber_scheduler::~ueber_scheduler()
{
  std::list<raw_pipe*>::iterator pipe_it;
  for ( pipe_it = pipes.begin();
  pipe_it != pipes.end();
  pipe_it++ )
  {
    delete( *pipe_it );
  }
}

void* scheduler::ueber_scheduler::stub_go(void* obj)
{
  ueber_scheduler::ueber_scheduler* scheduler_obj = ( ueber_scheduler::ueber_scheduler* )obj;
  pthread_barrier_wait( &scheduler_obj->barrier );
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Inherited from scheduler::abstract                                          /
////////////////////////////////////////////////////////////////////////////////

void 
scheduler::ueber_scheduler::spawn(fiber::fiber::ptr fiber)
{
  // znajdź uls z najmniejszym workloadem i zażądaj od niego inicjalizacji włókna.
  std::priority_queue< userspace_scheduler*, std::vector<userspace_scheduler*>, uls_comp > q;
  userspace_scheduler::list::iterator it;
  for ( it = schedulers.begin();
  it != schedulers.end();
  it++
  )
  {
    q.push( *it );
  }
  q.top()->spawn( fiber );
  blocked_num++;
}

/** \brief userspace_scheduler nie znalazł u siebie odpowiedniego wątku - trzeba zrobić broadcast.
 */
bool
scheduler::ueber_scheduler::send( spawned_data& data )
{
	std::list< raw_pipe* >::iterator i;
	for ( i = pipes.begin();
			  i != pipes.end();
				i++ )
	{
		spawned_data td;
		spawned_data::rewrite( td, data );
		while ( ! (*i)->write_in( td ) )
		{
		}
	}
	return true;
}

bool
scheduler::ueber_scheduler::receive( spawned_data& data )
{
	return true;
}

