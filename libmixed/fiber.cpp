#include <vector>

#include "fiber.hpp"
#include "scheduler.hpp"
#include "scheduler_tools.hpp"

fiber::fiber::fiber()
: _supervisor( 0 )
{
}

fiber::fiber::~fiber()
{
}

void 
fiber::fiber::init()
{
  state.on_init();
}

void 
fiber::fiber::run()
{
  go();
  state.on_exit();
}

void 
fiber::fiber::set_supervisor( scheduler::userspace_scheduler* supervisor_ )
{
  _supervisor = supervisor_;
}

void
fiber::fiber::spawn( fiber::fiber::ptr f )
{
  _supervisor->spawn( f, false );
}

bool
fiber::fiber::send( scheduler::spawned_data& message )
{
  bool rv;

	if ( _supervisor != 0 )
	{
		rv = _supervisor->send( message );
	}
	else
	{
		rv = false;
	}

  return rv;
}

bool
fiber::fiber::receive( scheduler::spawned_data& d )
{
  bool rv;
  std::list< scheduler::spawned_data >::iterator i = incoming_messages.begin();
  if ( i == incoming_messages.end() )
  {
    rv = false;
  }
  else
  {
    scheduler::spawned_data::rewrite( d, *i );
    rv = true;
		// usuń wiadomość z kolejki odebranych
		incoming_messages.pop_front();
  }
  return rv;
}

void
fiber::fiber::receive_data( scheduler::spawned_data& d )
{
  incoming_messages.push_back( d );
}

void
fiber::fiber::send_data( scheduler::spawned_data& d )
{
	// probably not needed
}

// wrappery dla socketów
bool
fiber::fiber::read( std::vector< char >& buf_ , ssize_t& read_bytes_, int fd_  )
{
	using namespace scheduler;

	_supervisor->read( buf_ , read_bytes_, this, fd_ );
	spawned_data result;
	if ( receive( result ) )
	{
		switch ( result.d )
		{
			case SOCKET_READ_READY:
				return ( read_bytes_ = ::read( fd_, &buf_[0], buf_.size() ) ) > 0;

			case SOCKET_READ_FAIL:
			default:
				return false;
		}
	}
	else
	{
		return false;
	}
}

bool
fiber::fiber::write( std::vector< char >& buf_ , ssize_t& written_bytes_, int fd_  )
{
	using namespace scheduler;

	_supervisor->write( buf_ , written_bytes_, this, fd_  );
	spawned_data result;
	if ( receive( result ) )
	{
		switch ( result.d )
		{
			case SOCKET_WRITE_READY:
				return ( written_bytes_ = ::write( fd_, &buf_[0], buf_.size() ) ) > 0;

			case SOCKET_WRITE_FAIL:
			default:
				return false;
		}
	}
	else
	{
		return false;
	}
}

int
fiber::fiber::accept( int fd_, ::sockaddr& saddr )
{
	using namespace scheduler;

  scheduler::accept_connect_data data;
	_supervisor->accept( fd_, this, &data );
	spawned_data result;
	if ( receive( result ) )
	{
		switch ( result.d )
		{
			case SERVER_ACCEPT_OK:
				saddr = data.saddr;
				return data.fd;

			case SERVER_ACCEPT_FAIL:
			default:
				return 0;
		}
	}
	else
	{
		return -1;
	}
}

// wrappery dla klienta
// note: po accept() klientem jest również otrzymany fd
bool
fiber::fiber::connect( int fd_, ::sockaddr& saddr )
{
	using namespace scheduler;

  scheduler::accept_connect_data data;
	_supervisor->connect( fd_, this, &data );
	spawned_data result;
	if ( receive( result ) )
	{
		switch ( result.d )
		{
			case CLIENT_CONNECT_OK:
				saddr = data.saddr;
				return true;

			case CLIENT_CONNECT_FAIL:
			default:
				return false;
		}
	}
	else
	{
		return false;
	}
}

bool
fiber::fiber::do_close( int fd_ )
{
  return _supervisor->close( fd_, this );
}
