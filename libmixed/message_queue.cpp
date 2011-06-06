#include "message_queue.hpp"
#include "scheduler_tools.hpp"
#include <list>
#include <iostream>
#include <errno.h>


////////////////////////////////////////////////////////////////////////////////
// message_queue                                                               /
////////////////////////////////////////////////////////////////////////////////
// private                                                                     /
////////////////////////////////////////////////////////////////////////////////
int
scheduler::message_queue::get_ready_queue( bool want_read )
{
	if ( want_read )
	{
		for (int i = 0; i < queue_count; i++ )
		{
			if ( ums[i] == MQ_READABLE )
			{
				return i;
			}
		}
	}
	else
	{
		for (int i = 0; i < queue_count; i++ )
		{
			if ( ums[i] == MQ_FREE )
			{
				return i;
			}
		}
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////
// public                                                                      /
////////////////////////////////////////////////////////////////////////////////
scheduler::message_queue::message_queue()
{
	ums[0] = ums[1] = MQ_FREE;
}

scheduler::message_queue::~message_queue()
{
}

bool
scheduler::message_queue::write( spawned_data& m )
{
  int current_queue = get_ready_queue();
  if ( current_queue != -1 )
  {
    messages_t& ml = mls[ current_queue ];
    ml.push_back( m );
		ums[ current_queue ] = MQ_READABLE;
    return true;
  }
  else
  {
    return false;
  }
}

bool
scheduler::message_queue::read( spawned_data& m )
{
  int current_queue = get_ready_queue();
  if ( current_queue != -1 )
  {
    messages_t& ml = mls[ current_queue ];

    for ( messages_t::iterator it = ml.begin();
        it != ml.end();
        it++
        )
    {
      spawned_data tm = *it;

      ml.erase( it );
      if ( ml.empty() )
      {
        ums[ current_queue ] = MQ_READY;
        ml.clear();
      }
      std::cout << "read " << ( ( m->d == END ) ? "END" : "SH..." ) << "; " 
        << "address: " << (unsigned long)&ml << ", size: " << ml.size() << std::endl;
      return true;
    }
  }
  else
  {
    get_writeable_queue();
    return false;
  }
}

