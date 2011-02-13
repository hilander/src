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
scheduler::raw_pipe::write_in( void* sp, size_t s )
{
  return write( in[1], sp, s );
}

int 
scheduler::raw_pipe::read_in( void* sp, size_t s )
{
  return read( in[0], sp, s );
}

int 
scheduler::raw_pipe::write_out( void* sp, size_t s )
{ 

  return write( out[1], sp, s );
}

int 
scheduler::raw_pipe::read_out( void* sp, size_t s )
{
  return read( out[0], sp, s );
}
