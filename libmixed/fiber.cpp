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

////////////////////////////////////////////////////////////////////////////////
// wrappery dla socketów
////////////////////////////////////////////////////////////////////////////////

bool
fiber::fiber::read( void* buf_ , ssize_t& read_bytes_, int fd_  )
{
	scheduler::read_write_data rwdata;
	rwdata.fd = fd_;
	rwdata.buf = buf_;
  rwdata.size = read_bytes_;
	return _supervisor->read( this, rwdata, read_bytes_ );
}

bool
fiber::fiber::write( void* buf_ , ssize_t& written_bytes_, int fd_  )
{
	scheduler::read_write_data rwdata;
	rwdata.fd = fd_;
	rwdata.buf = buf_;
  rwdata.size = written_bytes_;
	return _supervisor->write( this, rwdata, written_bytes_ );
}

bool
fiber::fiber::accept( int fd_, scheduler::accept_connect_data& data_ )
{
	return _supervisor->accept( fd_, &data_ );
}

// wrappery dla klienta
// note: po accept() klientem jest również otrzymany fd
bool
fiber::fiber::connect( int fd_, scheduler::accept_connect_data& data_ )
{
	return _supervisor->connect( fd_, &data_ );
}

void
fiber::fiber::do_close( int fd_ )
{
  _supervisor->close( fd_ );
}
