#include <socket_factory.hpp>
#include <poller.hpp>
#include <tr1/memory>
#include <vector>

using std::tr1::shared_ptr;
using std::vector;

scheduler::sockets::client::client( int fd_
                                  , shared_ptr< posix::sockaddr > sa_
                                  , ::socklen_t sa_len_
                                  , scheduler::poller::ptr p_ )
: _fd( fd_ )
, _sa( sa_ )
, _sa_len( sa_len_ )
, _p( p_ )
{
}

scheduler::sockets::client::ptr
scheduler::sockets::client::get_ptr( int fd_
                                   , shared_ptr< posix::sockaddr > sa_
                                   , ::socklen_t sa_len_
                                   , scheduler::poller::ptr p_ )
{
  client::ptr cp = new client( fd_, sa_, sa_len_, p_ );
  cp->init();
	return cp;
}

void
scheduler::sockets::client::init()
{
	do_register( _fd );
}

scheduler::sockets::client::~client()
{
}

int
scheduler::sockets::client::connect()
{
	return posix::connect( _fd, 0, 0 );
}

int
scheduler::sockets::client::read( vector< char >& buf )
{
  return ::read( _fd, &buf[0], buf.size() );
}

int
scheduler::sockets::client::write( vector< char >& buf )
{
	return ::write( _fd, &buf[0], buf.size() );
}

bool
scheduler::sockets::client::do_register( int fd_ )
{
	return _p->add( fd_ );
}

void
scheduler::sockets::client::deregister( int fd_ )
{
	_p->remove( fd_ );
}
