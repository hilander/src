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
  FIBER_SPECIFIC,
	NOTHING
};

struct spawned_data
{
  public:
    typedef spawned_data* ptr;
    data_kind d;
    void* p;
		fiber::fiber* f;
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
		bool write_in( spawned_data* sp );
		bool read_in( spawned_data* sp );
		bool write_out( spawned_data* sp );
		bool read_out( spawned_data* sp );
};

}
#endif
