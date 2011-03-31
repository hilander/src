#include "scheduler_tools.hpp"
#include "message_queue.hpp"
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// spawned_data                                                               //
////////////////////////////////////////////////////////////////////////////////

void 
scheduler::spawned_data::rewrite( spawned_data& target, spawned_data& source )
{
	target.d = source.d;
	target.p = source.p;
	target.sender = source.sender;
	target.receiver = source.receiver;
}

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
scheduler::raw_pipe::write_in( spawned_data& sp )
{
	//std::cout << "in: "; std::cout.flush();
  return in.write( sp );
}

bool 
scheduler::raw_pipe::read_in( spawned_data& sp )
{
	//std::cout << "in: "; std::cout.flush();
  return in.read( sp );
}

bool 
scheduler::raw_pipe::write_out( spawned_data& sp )
{ 
	//std::cout << "out: "; std::cout.flush();
  return out.write( sp );
}

bool 
scheduler::raw_pipe::read_out( spawned_data& sp )
{
	//std::cout << "out: "; std::cout.flush();
  return out.read( sp );
}
