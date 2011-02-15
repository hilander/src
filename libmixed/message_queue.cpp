#include "message_queue.hpp"
#include "scheduler_tools.hpp"
#include <list>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
// message                                                                     /
////////////////////////////////////////////////////////////////////////////////
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
			if ( mls[i].empty() == false )
			{
				return i;
			}
			else
			{
				ums[i] = ZEROED;
			}
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
scheduler::message_queue::read( message* m )
{
	messages_t& ml = get_readable_queue();

  for ( messages_t::iterator it = ml.begin();
      it != ml.end();
      it++
      )
  {
    message* tm = *it;
    if ( tm->used() == false )
    {
      m = tm;
      tm->set_used();
      return true;
    }
  }
  return false;
}
