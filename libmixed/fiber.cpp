#include <fiber.hpp>

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