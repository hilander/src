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
	return _supervisor->read( buf_ , read_bytes_, this, fd_ );
}

bool
fiber::fiber::write( std::vector< char >& buf_ , ssize_t& read_bytes_, int fd_  )
{
	return _supervisor->write( buf_ , read_bytes_, this, fd_  );
}

// wrappery dla serwera
bool
fiber::fiber::init_server( int fd_ )
{
	return _supervisor->init_server( fd_, this );
}

int
fiber::fiber::accept( int fd_ )
{
	return _supervisor->accept( fd_, this );
}

// wrappery dla klienta
// note: po accept() klientem jest również otrzymany fd
bool
fiber::fiber::init_client( int fd_ )
{
	return _supervisor->init_client( fd_, this );
}

bool
fiber::fiber::connect( int fd_ )
{
	return _supervisor->connect( fd_, this );
}
