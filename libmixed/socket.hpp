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

	using std::tr1::shared_ptr;
	using std::map;

class socket
{

	public: // typedefs
		typedef shared_ptr< socket > ptr;

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
		static socket::ptr instance;

	private:
		int _fd;
		map< int, shared_ptr< ::epoll_event > > watched_sockets;
};

}

#endif
