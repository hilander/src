#ifndef SCHEDULER_CONFIG_HPP
#define SCHEDULER_CONFIG_HPP

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <container.hpp>

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
		int in[2]; /// \brief Zapisuje ueber_scheduler, odczytuje userspace_scheduler.
		int out[2]; /// \brief Zapisuje userspace_scheduler, odczytuje ueber_scheduler.

	public:
		//ctor
		raw_pipe();

		//dtor
		~raw_pipe();

		void init()
		{
			pipe2( in, O_NONBLOCK );
			pipe2( out, O_NONBLOCK );
		}
	public:
		typedef raw_pipe* ptr;
		int write_in( void* sp, size_t s )
		{
			return write( in[1], sp, s );
		}

		int read_in( void* sp, size_t s )
		{
			return read( in[0], sp, s );
		}

		int write_out( void* sp, size_t s )
		{ 
			
			return write( out[1], sp, s );
		}

		int read_out( void* sp, size_t s )
		{
			return read( out[0], sp, s );
		}
};

}

#endif
