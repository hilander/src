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
scheduler::ueber_scheduler::init()
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
  blocked_num = 0;
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
      //std::cout << "started" << std::endl;
      spawned_data pc;
      pc.d = NOTHING;
      if ( read_and_interpret( *pipe_it, &pc ) )
      {
        //std::cout << "started.." << std::endl;
        switch ( pc.d )
        {
          case SPAWN_CONFIRMED:
            blocked_num--;
            //std::cout << "::" << (*pipe_it)->out[0] << ":: " << std::cout.flush();
            break;
            
          case BLOCK:
            break;
            
          default:
            break;
        }
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
  } while ( (total_workload != 0) || (blocked_num != 0) ) ;
  for ( it = schedulers.begin();
  it != schedulers.end();
  it++
  )
  (*it)->finish();
  //std::cout << "total workload: " << total_workload << ", blocked num: " << blocked_num << std::endl;
}

bool 
scheduler::ueber_scheduler::get_from_pipe(scheduler::raw_pipe* rp, scheduler::spawned_data* sp)
{
    return rp->read_out( sp );
}

bool 
scheduler::ueber_scheduler::read_and_interpret(scheduler::raw_pipe* rp, scheduler::spawned_data* pc)
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
