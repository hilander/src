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
	SPAWN_FROM_FIBER,

	NOTHING
};

struct accept_connect_data
{
	typedef accept_connect_data* ptr;

	int fd;
	::sockaddr saddr;
};

struct socket_req
{
	enum req
	{
		SOCKET_READ_REQ,
		SOCKET_WRITE_REQ,
		SOCKET_ACCEPT_REQ,
	};
	req request;
};

struct read_write_data
{
	typedef read_write_data* ptr;

	int fd;
	void* buf;
	ssize_t size;
	socket_req req;
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
