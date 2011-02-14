#include "message_queue.hpp"
#include "scheduler_tools.hpp"
#include <list>

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

////////////////////////////////////////////////////////////////////////////////
// message_queue                                                               /
////////////////////////////////////////////////////////////////////////////////
scheduler::message_queue::message_queue()
{
}

scheduler::message_queue::~message_queue()
{
}

void
scheduler::message_queue::clean()
{
  message candidate;
  std::list< message >::iterator erase_it = ml.end();

  for ( std::list< message >::iterator it = ml.begin();
      it != ml.end();
      it++
      )
  {
    if ( erase_it != ml.end() )
    {
      ml.erase( erase_it );
      erase_it = ml.end();
    }

    message m = *it;
    if ( m.used() )
    {
      erase_it = it;
    }
  }
}

