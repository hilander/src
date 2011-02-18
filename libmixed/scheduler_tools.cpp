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
	//cout << "write_in" << endl;
  message::ptr m = new message( sp );
  return in.write( m );
}

bool 
scheduler::raw_pipe::read_in( spawned_data* sp )
{
	//cout << "read_in" << endl;
  message::ptr m = new message( sp );
  return in.read( m );
}

bool 
scheduler::raw_pipe::write_out( spawned_data* sp )
{ 

	//cout << "write_out" << endl;
  message::ptr m = new message( sp );
  return out.write( m );
}

bool 
scheduler::raw_pipe::read_out( spawned_data* sp )
{
	//cout << "read_out" << endl;
  message::ptr m = new message( sp );
  return in.read( m );
}
