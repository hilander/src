#include <fiber.hpp>
#include <scheduler.hpp>
#include <scheduler_tools.hpp>

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

bool
fiber::fiber::send( scheduler::spawned_data*& message )
{
  bool rv;

	if ( _supervisor == 0 )
	{
		rv = _supervisor->send( message );
	}
	else
	{
		rv = false;
	}

  return rv;
}

void
fiber::fiber::receive_data( scheduler::spawned_data*& d )
{
  incoming_messages.push_back( d );
}

void
fiber::fiber::send_data( scheduler::spawned_data*& d )
{

}
