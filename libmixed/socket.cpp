#include "poller.hpp"
#include "socket.hpp"
#include "mutex_trylock.hpp"
#include <pthread.h>
#include <sys/epoll.h>

using std::tr1::shared_ptr;
const int ignored_epoll_value = 1024;

scheduler::socket::ptr scheduler::socket::instance( (scheduler::socket* )0 );

bool
scheduler::poller::poll()
{
	return true;
}

scheduler::poller::ptr
scheduler::poller::get()
{
	if ( instance.get() == 0 )
	{
		instance.reset( new poller() );
		instance->init();
	}

	return instance;
}

void
scheduler::poller::init()
{
  using scheduler::trylock;

  trylock tl( new shared_ptr< ::pthread_mutex_t >() );
  trylock* ptl = tl.get();
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
	ev->data.ptr = 0;
	ev->data.fd = fd_;
	ev->events = EPOLLIN | EPOLLOUT;

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
}
