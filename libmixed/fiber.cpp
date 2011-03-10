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

void 
fiber::fiber::set_supervisor( scheduler::abstract* supervisor_ )
{
  _supervisor = supervisor_;
}

bool
fiber::fiber::send( scheduler::spawned_data*& message )
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
fiber::fiber::receive( scheduler::spawned_data*& d )
{
  bool rv;
  std::list< scheduler::spawned_data* >::iterator i = incoming_messages.begin();
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
fiber::fiber::receive_data( scheduler::spawned_data*& d )
{
  incoming_messages.push_back( d );
}

void
fiber::fiber::send_data( scheduler::spawned_data*& d )
{

}
