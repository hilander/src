#include "scheduler_tools.hpp"

scheduler::raw_pipe::raw_pipe()
{
}

scheduler::raw_pipe::~raw_pipe()
{
	close( in[0] );
	close( in[1] );
	close( out[0] );
	close( out[1] );
}
