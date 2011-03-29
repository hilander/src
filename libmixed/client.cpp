#include <socket_factory.hpp>
#include <poller.hpp>
#include <tr1/memory>

using std::tr1::shared_ptr;

scheduler::sockets::client::client( int fd_
                                  , shared_ptr< posix_socket::sockaddr > sa_
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
                                   , shared_ptr< posix_socket::sockaddr > sa_
                                   , ::socklen_t sa_len_
                                   , scheduler::poller::ptr p_ )
{
  client::ptr cp = new client( fd_, sa_, sa_len_, p_ );
  cp->init();
	return cp;
}

int
scheduler::sockets::client::connect()
{
	return posix_socket::connect( _fd, 0, 0 );
}

int
scheduler::sockets::client::read()
{
  return 0; // stub
}

int
scheduler::sockets::client::write()
{
  return 0; // stub
}

void
scheduler::sockets::client::init()
{
  // stub
}

bool
scheduler::sockets::client::do_register( int fd_ )
{
	return _p->add( fd_ );
}

void
scheduler::sockets::client::deregister( int fd_ )
{
	return _p->remove( fd_ );
}
