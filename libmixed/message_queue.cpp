#include "message_queue.hpp"
#include "scheduler_tools.hpp"
#include <list>
#include <iostream>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
// message                                                                     /
////////////////////////////////////////////////////////////////////////////////
scheduler::message::message( spawned_data* sp )
{
  sd = sp;
  _used = false;
}

scheduler::message::message()
{
  _used = false;
}

scheduler::message::~message()
{
  if ( sd != 0 )
  {
    delete sd;
  }
}

bool
scheduler::message::used()
{
  return _used;
}

void
scheduler::message::set_used()
{
  _used = true;
}

////////////////////////////////////////////////////////////////////////////////
// message_queue                                                               /
////////////////////////////////////////////////////////////////////////////////
// private                                                                     /
////////////////////////////////////////////////////////////////////////////////
int
scheduler::message_queue::get_readable_queue()
{
	for (int i = 0; i < queue_count; i++ )
	{
		if ( ums[i] == READABLE )
		{
            return i;
		}
	}
	return EAGAIN;
}

int
scheduler::message_queue::get_writeable_queue()
{
	for (int i = 0; i < queue_count; i++ )
	{
		if ( ( ums[i] == ZEROED ) && ( ums[ (i+1)%2 ] == WRITEABLE ) )
		{
			ums[    i    ] = WRITEABLE;
			ums[ (i+1)%2 ] = READABLE;
			return i;
		}

		if ( ( ums[i] == ZEROED ) && ( ums[ (i+1)%2 ] == ZEROED ) )
		{
			ums[    i    ] = WRITEABLE;
			return i;
		}

		if ( ( ums[i] == WRITEABLE ) && ( ums[ (i+1)%2 ] == READABLE ) )
		{
			return i;
		}
	}
	return EAGAIN;
}

////////////////////////////////////////////////////////////////////////////////
// message_queue                                                               /
////////////////////////////////////////////////////////////////////////////////
// public                                                                      /
////////////////////////////////////////////////////////////////////////////////
scheduler::message_queue::message_queue()
{
	ums[0] = ums[1] = ZEROED;
}

scheduler::message_queue::~message_queue()
{
}

bool
scheduler::message_queue::write( message::ptr m )
{
	messages_t& ml = mls[ get_writeable_queue() ];
    ml.push_back( m );
    return true;
}

bool
scheduler::message_queue::read( message::ptr m )
{
    int current_queue = get_readable_queue();
	messages_t& ml = mls[ current_queue ];

  for ( messages_t::iterator it = ml.begin();
      it != ml.end();
      it++
      )
  {
    message* tm = *it;

    if ( tm == 0 ) 
    {
        m = 0;
        if ( ml.empty() )
        {
            std::cout << "dupa!" << std::endl;
            ums[ current_queue ] = ZEROED;
        }
        return false;
    }

    if ( tm->used() == false )
    {
      m = tm;
      tm->set_used();
      ml.erase( it );
      if ( ml.empty() )
      {
          ums[ current_queue ] = ZEROED;
            std::cout << "dupa!" << std::endl;
      }
      return true;
    }
  }
  return false;
}
