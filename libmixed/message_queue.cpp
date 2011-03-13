#include "message_queue.hpp"
#include "scheduler_tools.hpp"
#include <list>
#include <iostream>
#include <errno.h>


////////////////////////////////////////////////////////////////////////////////
// message_queue                                                               /
////////////////////////////////////////////////////////////////////////////////
// public                                                                      /
////////////////////////////////////////////////////////////////////////////////
scheduler::message_queue::message_queue()
{
  pthread_mutexattr_init( &_mattrs );
  pthread_mutex_init( &_mutex, &_mattrs );
}

scheduler::message_queue::~message_queue()
{
}

bool
scheduler::message_queue::write( spawned_data* m )
{
  bool rv = true;

  if ( pthread_mutex_trylock( &_mutex ) == 0 )
	{
		//std::cout << "read: mutex_lock..."; std::cout.flush();
		try
		{
			spawned_data::ptr tmp = new spawned_data();
			spawned_data::rewrite( tmp, m );
			_messages.push_back( tmp );
		}
		catch ( std::bad_alloc )
		{
			rv = false;
		}
		pthread_mutex_unlock( &_mutex );
		//std::cout << "mutex_UN_lock" << std::endl;
	}
	else
	{
		rv = false;
	}

  return rv;
}

bool
scheduler::message_queue::read( spawned_data* m )
{
  bool rv = true;

  if ( pthread_mutex_trylock( &_mutex ) == 0 )
	{
		//std::cout << "read: mutex_lock..."; std::cout.flush();
		if ( ! _messages.empty() )
		{
      scheduler::spawned_data::rewrite( m, _messages.front() );
			if ( m->d >= BLOCK && m->d <= NOTHING )
			{
				_messages.pop_front();
			}
		}
		else
		{
			rv = false;
		}
		pthread_mutex_unlock( &_mutex );
		//std::cout << "mutex_UN_lock" << std::endl;
	}
	else
	{
		rv = false;
	}

  return rv;
}
