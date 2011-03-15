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

class mutex
{
	public:

		mutex( std::tr1::shared_ptr< ::pthread_mutex_t > m_ );

		~mutex();

		bool trylock();

	private:

		mutex( const mutex& );

		bool _locked;

		std::tr1::shared_ptr< ::pthread_mutex_t > _m;
};

mutex* operator* ( mutex );

}

#endif
