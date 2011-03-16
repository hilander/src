#include "mutex_trylock.hpp"


scheduler::trylock* 
scheduler::trylock::operator* ()
{
  return 0;
	//throw scheduler::pointer_not_used();
}

scheduler::trylock::trylock( std::tr1::shared_ptr< ::pthread_mutex_t > m_ )
	: _m( m_ )
		, _locked( false )
{
}

scheduler::trylock::~trylock()
{
	if ( _locked )
	{
		pthread_mutex_unlock( _m.get() );
	}
}

bool
scheduler::trylock::try_lock()
{
	if ( pthread_mutex_trylock( _m.get() ) )
	{
		_locked = true;
	}
	return _locked;
}
