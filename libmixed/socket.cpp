#include "socket.hpp"
#include "mutex_trylock.hpp"
#include <pthread.h>
#include <sys/epoll.h>

using std::tr1::shared_ptr;
const int ignored_epoll_value = 1024;

scheduler::socket::ptr scheduler::socket::instance( (scheduler::socket* )0 );

bool
scheduler::socket::poll()
{
	return true;
}

scheduler::socket::ptr
scheduler::socket::get()
{
	if ( instance.get() == 0 )
	{
		instance.reset( new socket() );
		instance->init();
	}

	return instance;
}

void
scheduler::socket::init()
{
	//init epoll
	_fd = epoll_create( ignored_epoll_value );
	if ( _fd == -1 )
	{
		throw std::exception();
	}
}

bool
scheduler::socket::add( int fd_ )
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
scheduler::socket::remove( int fd_ )
{
}
