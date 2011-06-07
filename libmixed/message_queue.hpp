#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <list>
#include <pthread.h>

namespace scheduler
{

struct spawned_data;

const int queue_count = 2;

class message_queue
{
	private:
		enum use_mode { MQ_FREE, MQ_READABLE, MQ_WRITABLE };
		typedef std::list< spawned_data > messages_t;
		messages_t mls[ queue_count ];
		use_mode ums[ queue_count ];
		int get_ready_queue( bool want_read );
    short get_state();

	public:
		message_queue();
		~message_queue();
		void clean();

		bool read( spawned_data& m );
		bool write( spawned_data& m );
};

}

#endif
