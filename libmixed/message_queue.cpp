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
      std::cout << "writeable is " << (i+1)%2 << std::endl;
			return i;
		}

		if ( ( ums[i] == WRITEABLE ) && ( ums[ (i+1)%2 ] == READABLE ) )
		{
			return i;
		}
	}
  std::cout << "writeable(9) is " << std::endl;
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
scheduler::message_queue::write( spawned_data* m )
{
  int current_queue = get_writeable_queue();
  if ( current_queue != EAGAIN )
  {
    messages_t& ml = mls[ current_queue ];
    if ( m != 0 )
    {
      ml.push_back( m );
      //std::cout << "written " << ( ( m->d == END ) ? "END" : "SH..." ) << "; " 
        //<< "address: " << (unsigned long)&ml << ", size: " << ml.size() << std::endl;
    }
    return true;
  }
  else
  {
    return false;
  }
}

bool
scheduler::message_queue::read( spawned_data* m )
{
  int current_queue = get_readable_queue();
  if ( current_queue != EAGAIN )
  {
    messages_t& ml = mls[ current_queue ];

    for ( messages_t::iterator it = ml.begin();
        it != ml.end();
        it++
        )
    {
      spawned_data* tm = *it;

      m->d = tm->d;
      m->p = tm->p;
      ml.erase( it );
      if ( ml.empty() )
      {
        ums[ current_queue ] = ZEROED;
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
