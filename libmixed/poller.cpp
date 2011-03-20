//#include <iostream>
#include "poller.hpp"
#include "mutex_trylock.hpp"
#include <pthread.h>
#include <sys/epoll.h>
#include <vector>

using std::tr1::shared_ptr;
using std::map;
using std::vector;

const int ignored_epoll_value = 1024;

const int initial_epolls = 128;

// man epoll_wait(2):
// timeout = 0: return immediately even if no events are available
const int epoll_timeout = 0;

//static member
scheduler::poller::ptr scheduler::poller::instance( (scheduler::poller* )0 );

scheduler::poller::ptr
scheduler::poller::get( ::pthread_mutex_t* m_ )
{
	if ( instance.get() == 0 )
	{
		instance.reset( new poller( m_ ) );
		instance->init();
	}

	return instance;
}

scheduler::poller::poller( ::pthread_mutex_t* m_ )
  : _m( m_ )
  , watched_sockets_size( initial_epolls )
  , watched_sockets( new ::epoll_event[ initial_epolls ]() )
{
}

scheduler::poller::poller()
{}

scheduler::poller::poller( scheduler::poller& )
{}

scheduler::poller::~poller()
{
  delete[] watched_sockets;
}

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
scheduler::poller::add( int fd_ ) throw( std::exception)
{
  if ( current_sockets_number == watched_sockets_size )
  {
    throw std::exception();
  }

  std::pair<int, ::epoll_event > fd_pair( fd_, ::epoll_event() );
  current_sockets_number++;

	fd_pair.second.events = EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP;
	fd_pair.second.data.fd = fd_;

	if ( ::epoll_ctl( _fd, EPOLL_CTL_ADD, fd_, &(fd_pair.second) ) == 0 )
	{
    _events.insert( fd_pair );
		return true;
	}
	else
	{
		return false;
	}
}

vector< ::epoll_event >*
scheduler::poller::poll()
{
  int events_number = epoll_wait( _fd, watched_sockets, watched_sockets_size, epoll_timeout );
  
  vector< ::epoll_event >* v = new vector< ::epoll_event>( events_number );

  for ( int i = 0; i < events_number; i++ )
  {
    ( *v )[ i ].events = watched_sockets[ i ].events;
    ( *v )[ i ].data.fd = watched_sockets[ i ].data.fd;
  }
  
	return v;
}

void
scheduler::poller::remove( int fd_ )
{
  map< int, ::epoll_event>::iterator removed = _events.find( fd_ );

  ::epoll_ctl( _fd, EPOLL_CTL_DEL, fd_, &( removed->second ) );


  if ( removed != _events.end() )
  {
  _events.erase( removed );
  }

  current_sockets_number--;

}
