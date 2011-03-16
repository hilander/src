#ifndef LIBMIXED_SOCKET_HPP
#define LIBMIXED_SOCKET_HPP

#include <tr1/memory>
#include <map>
#include <sys/epoll.h>

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

		static ptr get();

		/** \brief Triggeruj epoll-a: sprawdź, które sockety coś zapisały / odczytały
		 * \return true, gdy co najmniej jeden z socketów zmienił stan; false wpw.
		 */
		bool poll();

		bool add( int fd_ );

		void remove( int fd_ );

	public:
		void init();

	private:
		static poller::ptr instance;

	private:
		int _fd;
		std::map< int, std::tr1::shared_ptr< ::epoll_event > > watched_sockets;
};

}

#endif
