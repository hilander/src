#ifndef SCHEDULER_CONFIG_HPP
#define SCHEDULER_CONFIG_HPP

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <container.hpp>
#include <sys/socket.h>
#include "message_queue.hpp"

namespace fiber
{
  class fiber;
}

namespace scheduler
{

class userspace_scheduler;

typedef container< fiber::fiber, std::map< fiber::fiber*, fiber::fiber* > > thread_container;

enum data_kind
{
	BLOCK,
  BLOCKED,
	UNBLOCKED,
	END,
	SPAWN,
	SPAWN_CONFIRMED,
  FIBER_SPECIFIC,

	REGISTER_SERVER_REQ,
	REGISTER_SERVER_OK,
	REGISTER_SERVER_FAIL,

	REGISTER_CLIENT_REQ,
	REGISTER_CLIENT_OK,
	REGISTER_CLIENT_FAIL,

	DEREGISTER_SOCKET_REQ,
	DEREGISTER_SOCKET_OK,
	DEREGISTER_SOCKET_FAIL,

	SERVER_ACCEPT_REQ,
	SERVER_ACCEPT_OK,
	SERVER_ACCEPT_FAIL,

	CLIENT_CONNECT_REQ,
	CLIENT_CONNECT_OK,
	CLIENT_CONNECT_FAIL,

	SOCKET_READ_REQ,
	SOCKET_READ_READY,
	SOCKET_READ_FAIL,

	SOCKET_WRITE_REQ,
	SOCKET_WRITE_READY,
	SOCKET_WRITE_FAIL,

	NOTHING
};

struct accept_connect_data
{
	typedef accept_connect_data* ptr;

	int fd;
	::sockaddr saddr;
};

struct spawned_data
{
  public:
    typedef spawned_data* ptr;
    data_kind d;            // message kind
    void* p;                // additional data
		fiber::fiber* sender;
		fiber::fiber* receiver;
		userspace_scheduler* supervisor;

	public:
		static void rewrite( spawned_data& target, spawned_data& source );
};

/** \brief <i>Surowy</i> kanał do komunikacji.
 * Kanał udostępnia funkcje wysyłania / odbierania
 * poprzez systemowe potoki nienazwane (pipes).
 */
class raw_pipe
{
	private:
		message_queue in; /// \brief Zapisuje ueber_scheduler, odczytuje userspace_scheduler.
		message_queue out; /// \brief Zapisuje userspace_scheduler, odczytuje ueber_scheduler.

	public:
		//ctor
		raw_pipe();
    void init();

		//dtor
		~raw_pipe();

	public:
		typedef raw_pipe* ptr;
		bool write_in( spawned_data& sp );
		bool read_in( spawned_data& sp );
		bool write_out( spawned_data& sp );
		bool read_out( spawned_data& sp );
};

}
#endif
