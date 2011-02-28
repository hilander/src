#include <fiber.hpp>
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
  bool rv = false;
  return rv;
}
