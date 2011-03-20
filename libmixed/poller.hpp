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

		static ptr get( ::pthread_mutex_t* m_ );

		/** \brief Triggeruj epoll-a: sprawdź, które sockety coś zapisały / odczytały
		 * \return true, gdy co najmniej jeden z socketów zmienił stan; false wpw.
		 */
		std::vector< ::epoll_event >* poll();

		bool add( int fd_, uint32_t flags ) throw( std::exception);

		bool add( int fd_ ) throw( std::exception);

		void remove( int fd_ );

	public:

		void init();

    ~poller();

  private:

    poller();

    poller( poller& );

    poller( ::pthread_mutex_t* m_ );

	private:

		static poller::ptr instance;

	private:
		int _fd;
		::epoll_event* watched_sockets;
    size_t watched_sockets_size;
    size_t current_sockets_number;
    ::pthread_mutex_t* _m;
    std::map< int, ::epoll_event > _events;
};

}

#endif
