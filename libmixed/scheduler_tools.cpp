#include "scheduler_tools.hpp"
#include "message_queue.hpp"
#include <iostream>

using namespace std;

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

bool
scheduler::raw_pipe::write_in( spawned_data* sp )
{
  return in.write( sp );
}

bool 
scheduler::raw_pipe::read_in( spawned_data* sp )
{
  return in.read( sp );
}

bool 
scheduler::raw_pipe::write_out( spawned_data* sp )
{ 

  return out.write( sp );
}

bool 
scheduler::raw_pipe::read_out( spawned_data* sp )
{
  return out.read( sp );
}
