#include <socket_factory.hpp>
#include <poller.hpp>

namespace posix_socket
{
#include <sys/socket.h>
}

scheduler::sockets::client::ptr
scheduler::sockets::client::get_ptr()
{
	posix_socket::connect( 0, 0, 0 );
	return 0;
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
