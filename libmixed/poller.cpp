#include "poller.hpp"
#include "socket.hpp"
#include "mutex_trylock.hpp"
#include <pthread.h>
#include <sys/epoll.h>

using std::tr1::shared_ptr;
using std::map;

const int ignored_epoll_value = 1024;

// man epoll_wait(2):
// timeout = 0: return immediately even if no events are available
const int epoll_timeout = 0;

//static member
scheduler::poller::ptr scheduler::poller::instance( (scheduler::poller* )0 );

map< int, uint32_t >
scheduler::poller::poll()
{
  events.clear();

  epoll_wait( _fd, watched_sockets, watched_sockets.size(), epoll_timeout );
  
	return events;
}

scheduler::poller::ptr
scheduler::poller::get( shared_ptr< ::pthread_mutex_t >& m_ )
{
	if ( instance.get() == 0 )
	{
		instance.reset( new poller( m_ ) );
		instance->init();
	}

	return instance;
}

scheduler::poller::poller( shared_ptr< ::pthread_mutex_t >& m_ )
  : _m( m_ )
{
}

scheduler::poller::poller()
{}

scheduler::poller::poller( scheduler::poller& )
{}

void
scheduler::poller::init()
{
  using scheduler::trylock;

	//init epoll
	_fd = epoll_create( ignored_epoll_value );
	if ( _fd == -1 )
	{
		throw std::exception();
	}
}

bool
scheduler::poller::add( int fd_ )
{
	shared_ptr< ::epoll_event > ev( new ::epoll_event() );
	ev->data.fd = fd_;

	if ( ::epoll_ctl( _fd, EPOLL_CTL_ADD, fd_, ev.get() ) == 0 )
	{
		watched_sockets[ fd_ ] = ev;
		return true;
	}
	else
	{
		return false;
	}
}

void
scheduler::poller::remove( int fd_ )
{
  map< int, std::tr1::shared_ptr< ::epoll_event > >::iterator it = watched_sockets.find( fd_ );
  watched_sockets.erase( it );
}
