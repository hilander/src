#include <socket_factory.hpp>
#include <poller.hpp>
#include <tr1/memory>
#include <vector>

using std::tr1::shared_ptr;
using std::vector;

scheduler::sockets::server::server( int fd_
                                  , shared_ptr< posix::sockaddr > sa_
                                  , ::socklen_t sa_len_
                                  , scheduler::poller::ptr p_ )
: _fd( fd_ )
, _sa( sa_ )
, _sa_len( sa_len_ )
, _p( p_ )
{
}

scheduler::sockets::server::ptr
scheduler::sockets::server::get_ptr( int fd_
                                   , shared_ptr< posix::sockaddr > sa_
                                   , ::socklen_t sa_len_
                                   , scheduler::poller::ptr p_ )
{
  server::ptr cp = new server( fd_, sa_, sa_len_, p_ );
  cp->init();
	return cp;
}

void
scheduler::sockets::server::init()
{
}

scheduler::sockets::server::~server()
{
}

int
scheduler::sockets::server::accept()
{
	int accepted = posix::accept( _fd, 0, 0 );
	if ( accepted > 0 )
	{
		do_register( accepted );
	}
	return accepted;
}

void
scheduler::sockets::server::cancel( int fd_ )
{
	deregister( fd_ );
}

int
scheduler::sockets::server::read( vector< char >& buf )
{
  return ::read( _fd, &buf[0], buf.size() );
}

int
scheduler::sockets::server::write( vector< char >& buf )
{
	return ::write( _fd, &buf[0], buf.size() );
}

bool
scheduler::sockets::server::do_register( int fd_ )
{
	return _p->add( fd_ );
}

void
scheduler::sockets::server::deregister( int fd_ )
{
	_p->remove( fd_ );
}
