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

  pthread_mutex_lock( &_mutex );
  try
  {
    _messages.push_back( m );
  }
  catch ( std::bad_alloc )
  {
    rv = false;
  }
  pthread_mutex_unlock( &_mutex );

  return rv;
}

bool
scheduler::message_queue::read( spawned_data* m )
{
  bool rv = true;

  pthread_mutex_lock( &_mutex );
  if ( ! _messages.empty() )
  {
    m = _messages.front();
    _messages.pop_front();
  }
  else
  {
    rv = false;
  }
  pthread_mutex_unlock( &_mutex );

  return rv;
}
