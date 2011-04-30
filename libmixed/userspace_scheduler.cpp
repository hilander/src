#include <cassert>
#include <algorithm>
#include <netinet/in.h>
#include "userspace_scheduler.hpp"
#include "scheduler.hpp"
#include "poller.hpp"

scheduler::userspace_scheduler::userspace_scheduler(ueber_scheduler* ptr)
: workload(0), us(ptr), _ended( false )
{
}

/* Tylko do debugowania */
scheduler::userspace_scheduler::userspace_scheduler(ueber_scheduler* ptr, std::list< fiber::fiber::ptr > fibers_ )
: workload(0), us(ptr), _ended( false )
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
    epolls = 0;
    _epoller = poller::get();
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
scheduler::userspace_scheduler::do_epolls()
{
	using std::vector;
	using std::map;
	
  if ( epolls == 0 )
  {
    return;
  }

	epoll_state = _epoller->poll();

  if ( epoll_state == 0 )
  {
    return;
  }

	for ( vector< ::epoll_event >::iterator it = epoll_state->begin()
			; it != epoll_state->end()
			; it++ )
	{
		map< int, fiber::fiber::ptr >::iterator found = blocked.find( it->data.fd );
		if ( found != blocked.end() )
		{
      //std::cout << "." << it->data.fd ; std::cout.flush();
			found->second->state.unblock();
			blocked.erase( found );
		}
	}
}

void 
scheduler::userspace_scheduler::run()
{
  while ( !finished() )
  {
		do_epolls();
    read_messages();
    
    fiber::fiber::ptr running = ready.get();
    if ( running != 0  
			 && ( running->state.get() != libcoro::state_controller::BLOCKED ) )
    {
      running->start(this);
      if ( running->state.get() == libcoro::state_controller::FINISHED )
      {
        ready.dispose();
        workload--;
      }
    }
  }
	//std::cout << "userspace_scheduler::run(): ended." << std::endl;
	_ended = true;
}

void 
scheduler::userspace_scheduler::start()
{
  libcoro::coroutine::start( base_coroutine ); 
}

void 
scheduler::userspace_scheduler::spawn(fiber::fiber::ptr fiber)
{
  spawned_data sp;
  sp.d = SPAWN;
  sp.p = fiber;
  bool written;
  do
  {
    written = message_device->write_in( sp );
  }
  while ( !written )
		; // a co!
	//std::cout << "spawn(ueber): Done." << std::endl;
}


void 
scheduler::userspace_scheduler::spawn(void* f, bool confirm )
{
  fiber::fiber::ptr fiber = (fiber::fiber::ptr)f;
  
  // przydziel stos dla włókna
  manager->get( fiber );
  
  ready.insert( fiber );
  
  fiber->set_supervisor( this );
  // Obciążenie: +1;
  workload++;

  // spawn wywołany z zarządzanego włókna, nie wymaga potwierdzenia.
  if ( !confirm )
  {
    return;
  }

  //std::cout << workload << std::endl;
  spawned_data sp;
  sp.d = SPAWN_CONFIRMED;
  sp.p = f;
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
*/
void
scheduler::userspace_scheduler::block( fiber::fiber::ptr f, scheduler::read_write_data& d )
{
	f->state.block();
	blocked.insert( std::pair< int, fiber::fiber::ptr >( d.fd, f ) );
}

void 
scheduler::userspace_scheduler::read_messages()
{
  spawned_data sp;
  sp.d = NOTHING;
  sp.p = 0;
  
  if ( message_device->read_in( sp ) )
  {
		spawned_data response;
    switch (sp.d)
    {
      case BLOCKED:
        workload--;
        break;

      case END:
        finish();
        break;

      case SPAWN:
        spawn(sp.p, true);
				response.d = SPAWN_CONFIRMED;
				response.p = 0;
        break;

			case FIBER_SPECIFIC:
        if ( ready.exists( sp.receiver ) ) 
        {
          // Receiver fiber must read its data:
          sp.receiver->receive_data( sp );
        }
				break;

      default:
        break;
    }
  }
}

bool 
scheduler::userspace_scheduler::send( spawned_data& data )
{
  if ( data.receiver != 0 ) 
  {
		if ( ready.exists( data.receiver ) ) 
		{
			// Receiver fiber must read its data:
			data.receiver->receive_data( data );
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
scheduler::userspace_scheduler::receive( spawned_data& data )
{
  return false;
}

bool
scheduler::userspace_scheduler::ended()
{
	return _ended;
}

struct state_finder
{
	state_finder( int fd_ )
		: fd( fd_ )
	{
	}
	bool operator() ( ::epoll_event& ev )
	{
		return ev.data.fd == fd;
	}

	private:
	int fd;
};

bool
scheduler::userspace_scheduler::read( fiber::fiber::ptr caller, read_write_data& data_, ssize_t& read_bytes_ )
{
	using std::vector;
	
  assert( _epoller->contains( data_.fd ) );
	data_.req.request = socket_req::SOCKET_READ_REQ;
	block( (fiber::fiber*)caller, data_ );
	caller->yield();

	// we are unblocked!
	vector< ::epoll_event >::iterator ev = std::find_if( epoll_state->begin(), epoll_state->end(), state_finder( data_.fd ) );
	if ( ev->events & EPOLLIN )
	{
		void* tmp = data_.buf;
		read_bytes_ = ::read( data_.fd, tmp, data_.size );
		return (read_bytes_ > 0) ? true : false;
	}
	return false;
}
                                                                
bool
scheduler::userspace_scheduler::write( fiber::fiber::ptr caller, read_write_data& data_, ssize_t& written_bytes_ )
{
	using std::vector;
	
  assert( _epoller->contains( data_.fd ) );
	data_.req.request = socket_req::SOCKET_WRITE_REQ;
	block( (fiber::fiber*)caller, data_ );
	caller->yield();

	// we are unblocked!
	vector< ::epoll_event >::iterator ev = std::find_if( epoll_state->begin(), epoll_state->end(), state_finder( data_.fd ) );
	if ( ev->events & EPOLLOUT )
	{
		void* tmp = data_.buf;
		written_bytes_ = ::write( data_.fd, tmp, data_.size );
    std::cout << "," ; std::cout.flush();
		return (written_bytes_ > 0) ? true : false;
	}
	return false;
}

void
scheduler::userspace_scheduler::init_server( int fd_ )
{
  _epoller->add( fd_ );
  epolls++;
}

bool
scheduler::userspace_scheduler::accept( int fd_, accept_connect_data::ptr data )
{
  int accepted = 0;

  socklen_t sl = sizeof( ::sockaddr_in );
  if ( ( accepted = ::accept4( data->fd, &(data->saddr), &sl, SOCK_NONBLOCK ) ) != 0 )
  {
    data->fd = accepted;
    return true;
  }
  else
  {
    return false;
  }
}

void
scheduler::userspace_scheduler::init_client( int fd_ )
{
  _epoller->add( fd_ );
  epolls++;
}

bool
scheduler::userspace_scheduler::connect( int fd_, accept_connect_data::ptr data )
{
  return ( ::connect( data->fd, &data->saddr, sizeof(::sockaddr_in) ) == 0 );
  std::cout << "."; std::cout.flush();
}

void
scheduler::userspace_scheduler::close( int fd_ )
{
  _epoller->remove( fd_ );
  ::close( fd_ );
  epolls--;
}

