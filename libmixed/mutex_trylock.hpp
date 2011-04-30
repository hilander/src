#ifndef LIBMIXED_MUTEX_HPP
#define LIBMIXED_MUTEX_HPP

#include <pthread.h>
#include <exception>
#include <tr1/memory>

namespace scheduler
{

class pointer_not_used : public std::exception
{
};

class trylock
{
	public:

		trylock( std::tr1::shared_ptr< ::pthread_mutex_t > m_ );

		~trylock();

		bool try_lock();

	private:

		trylock( const trylock& );

		std::tr1::shared_ptr< ::pthread_mutex_t > _m;
    
		bool _locked;

    trylock* operator* ();
};


}

#endif
