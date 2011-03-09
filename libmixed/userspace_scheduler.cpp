#include "userspace_scheduler.hpp"
#include "scheduler.hpp"

scheduler::userspace_scheduler::userspace_scheduler(ueber_scheduler* ptr)
: workload(0), us(ptr)
{
}

/* Tylko do debugowania */
scheduler::userspace_scheduler::userspace_scheduler(ueber_scheduler* ptr, std::list< fiber::fiber::ptr > fibers_ )
: workload(0), us(ptr)
{
    for ( std::list< fiber::fiber::ptr >::iterator i = fibers_.begin();
          i != fibers_.end();
          i++ )
    {
        fiber::fiber::ptr f = *i;
        manager->get( f );

        ready.insert( f );

        // Obciążenie: +1;
        workload++;
    }
}

void 
scheduler::userspace_scheduler::init( raw_pipe::ptr message_pipe )
{
    base_coroutine = libcoro::factory::create_coroutine();
    libcoro::factory::assign_coroutine( this );
    if ( us == 0 )
    {
        manager = new libmanager::manager();
        manager->init();
    }
    else
    {
        manager = us->get_manager();
    }

    if ( message_pipe != 0 )
    {
        message_device = message_pipe;
    }
    else
    {
        message_device = 0;
    }
    scheduler_end = false;
    workload = ready.size();

    pthread_attr_init( &uls_attr );
    // inicjalizacja zakończona: można uruchomić wątek, w którym będzie działać scheduler.
    pthread_create( &uls_thread, &uls_attr, &userspace_scheduler::go, (void*)this );
}


void* 
scheduler::userspace_scheduler::go( void* data )
{
  userspace_scheduler::ptr usp = (userspace_scheduler::ptr) data;
  usp->start();
  return 0;
}

void scheduler::userspace_scheduler::join(fiber::fiber::ptr f)
{
  pthread_join( uls_thread, 0 );
}

bool 
scheduler::userspace_scheduler::finished()
{
  return scheduler_end;
}

void
scheduler::userspace_scheduler::finish()
{
  scheduler_end = true;
}

void 
scheduler::userspace_scheduler::run()
{
  while ( !finished() )
  {
    read_messages();
    
    fiber::fiber::ptr running = ready.get();
    if ( running != 0 )
    {
      running->start(this);
      if ( running->state.get() == libcoro::state_controller::FINISHED )
      {
        ready.dispose();
        workload--;
				//std::cout << "userspace_scheduler::run: "
				//	<< "workload: " << workload
				//	<< std::endl;
      }
    }
  }
}

void 
scheduler::userspace_scheduler::start()
{
  libcoro::coroutine::start( base_coroutine ); 
}

void 
scheduler::userspace_scheduler::spawn(fiber::fiber::ptr fiber)
{
  spawned_data* sp = new spawned_data();
  sp->d = SPAWN;
  sp->p = fiber;
  bool written;
  do
  {
    written = message_device->write_in( sp );
  }
  while ( !written )
		; // a co!
	//std::cout << "spawn: Done." << std::endl;
}


void 
scheduler::userspace_scheduler::spawn(void* f, int )
{
  fiber::fiber::ptr fiber = (fiber::fiber::ptr)f;
  
  // przydziel stos dla włókna
  manager->get( fiber );
  
  ready.insert( fiber );
  
  // Obciążenie: +1;
  workload++;
  //std::cout << workload << std::endl;
  spawned_data* sp = new spawned_data();
  sp->d = SPAWN_CONFIRMED;
  sp->p = f;
  bool written;
  do
  {
    written = message_device->write_out( sp );
  }
  while ( !written )
    ;
	//std::cout << "spawn: confirmed." << std::endl;
}

bool 
scheduler::userspace_scheduler::empty()
{
  return ready.empty();
}


int 
scheduler::userspace_scheduler::get_workload()
{
  return workload;
}


/*
* Technicznie: przerzuć wątek do puli zablokowanych wątków,
* która znajduje się w&nbsp;ueber_scheduler.
* TODO
*/
void 
scheduler::userspace_scheduler::block(fiber::fiber::ptr f)
{
  send_message( BLOCK, (void*)(&f) );
  // wątek nie powinien być wznawiany
  
}

/** Niebezpieczne złogi!
 */
void 
scheduler::userspace_scheduler::send_message(scheduler::data_kind k, void* d)
{
  char buf[ sizeof(data_kind) + sizeof(void*) ];
  size_t length = 0;
  switch ( k )
  {
    case BLOCK:
      memcpy( (void*)buf, (void*)(&k), (length = sizeof(data_kind)) );
      length += sizeof(void*);
      memcpy( (void*)(buf+length), (void*)(&d), length );
      break;
    default:
      break;
  }
  bool written;
  do
  {
    written = message_device->write_out( reinterpret_cast< spawned_data* >( buf ) );
  }
  while ( !written )
		;
}

void 
scheduler::userspace_scheduler::read_messages()
{
  size_t record_size = sizeof( sizeof(data_kind) + sizeof(void*) );
  spawned_data sp = { NOTHING, 0 };
  
  if ( message_device->read_in( &sp ) )
  {
		spawned_data response;
    switch (sp.d)
    {
      case END:
        finish();
        break;

      case SPAWN:
        spawn(sp.p, 0);
				response.d = SPAWN_CONFIRMED;
				response.p = 0;
        break;

			case FIBER_SPECIFIC:
				send( &sp );
				break;

      default:
        break;
    }
  }
}

bool 
scheduler::userspace_scheduler::send( spawned_data::ptr data )
{
  if ( data->receiver != 0 ) 
  {
		if ( ready.exists( data->receiver ) ) 
		{
			// Receiver fiber must read its data:
			data->receiver->receive_data( data );
			return true;
		}
		else
		{
			return message_device->write_out( data );
		}
  }
  return false;
}

// not needed?
bool
scheduler::userspace_scheduler::receive( spawned_data::ptr data )
{
  return false;
}

////////////////////////////////////////////////////////////////////////////////
// Only for debug purposes                                                     /
////////////////////////////////////////////////////////////////////////////////

// #ifdef USERSPACE_SCHEDULER_DEBUG

