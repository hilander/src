#ifndef SCHEDULER_CONFIG_HPP
#define SCHEDULER_CONFIG_HPP

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <container.hpp>
#include "message_queue.hpp"

namespace fiber
{
  class fiber;
}

namespace scheduler
{

typedef container< fiber::fiber, std::map< fiber::fiber*, fiber::fiber* > > thread_container;

enum data_kind
{
	BLOCK,
	END,
	SPAWN,
	SPAWN_CONFIRMED,
	NOTHING
};

struct spawned_data
{
  public:
    typedef spawned_data* ptr;
    data_kind d;
    void* p;
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
		int write_in( void* sp, size_t s );
		int read_in( void* sp, size_t s );
		int write_out( void* sp, size_t s );
		int read_out( void* sp, size_t s );
};

}
#endif
