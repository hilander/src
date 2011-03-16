#ifndef LIBMIXED_SOCKET_HPP
#define LIBMIXED_SOCKET_HPP

#include <tr1/memory>
#include <map>
#include <vector>
#include <sys/epoll.h>
#include <pthread.h>

namespace scheduler
{

struct socket_state
{
	public:
		enum {
			IDLE,
			GOT_DATA,
			WRITTEN_DATA,
			EXCEPTION,
			UNDEFINED
		};
};

class poller
{

	public: // typedefs
		typedef std::tr1::shared_ptr< poller > ptr;

	public:

		static ptr get( std::tr1::shared_ptr< ::pthread_mutex_t >& m_ );

		/** \brief Triggeruj epoll-a: sprawdź, które sockety coś zapisały / odczytały
		 * \return true, gdy co najmniej jeden z socketów zmienił stan; false wpw.
		 */
		std::map< int, uint32_t > poll();

		bool add( int fd_ );

		void remove( int fd_ );

	public:

		void init();

  private:

    poller();

    poller( poller& );

    poller( std::tr1::shared_ptr< ::pthread_mutex_t >& m_ );

	private:

		static poller::ptr instance;

	private:
		int _fd;
		std::vector< std::tr1::shared_ptr< ::epoll_event > > watched_sockets;
    std::tr1::shared_ptr< ::pthread_mutex_t > _m;
    std::map< int, uint32_t > events;
};

}

#endif
