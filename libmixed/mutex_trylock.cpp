#include "mutex_trylock.hpp"


scheduler::mutex* operator* ( scheduler::mutex& ) throw ( scheduler::pointer_not_used )
{
	throw scheduler::pointer_not_used();
}

scheduler::mutex::mutex( std::tr1::shared_ptr< ::pthread_mutex_t > m_ )
	: _m( m_ )
		, _locked( false )
{
}

scheduler::mutex::~mutex()
{
	if ( _locked )
	{
		pthread_mutex_unlock( _m.get() );
	}
}

bool
scheduler::mutex::trylock()
{
	if ( pthread_mutex_trylock( _m.get() ) )
	{
		_locked = true;
	}
	return _locked;
}
