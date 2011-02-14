#include "scheduler_tools.hpp"
#include "message_queue.hpp"

////////////////////////////////////////////////////////////////////////////////
// raw_pipe                                                                   //
////////////////////////////////////////////////////////////////////////////////
scheduler::raw_pipe::raw_pipe()
{
}

scheduler::raw_pipe::~raw_pipe()
{
}

void 
scheduler::raw_pipe::init()
{
}

int
scheduler::raw_pipe::write_in( spawned_data* sp )
{
  return 0;
}

int 
scheduler::raw_pipe::read_in( spawned_data* sp )
{
  return 0;
}

int 
scheduler::raw_pipe::write_out( spawned_data* sp )
{ 

  return 0;
}

int 
scheduler::raw_pipe::read_out( spawned_data* sp )
{
  return 0;
}
