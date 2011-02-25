#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <list>

namespace scheduler
{

struct spawned_data;

const int queue_count = 2;

class message_queue
{
    private:
        enum use_mode { ZEROED, READABLE, WRITEABLE };
        typedef std::list< spawned_data* > messages_t;
        messages_t mls[ queue_count ];
        use_mode ums[ queue_count ];
        int get_readable_queue();
        int get_writeable_queue();
    public:
        message_queue();
        ~message_queue();
        void clean();
        // TODO:
        bool read( spawned_data* m );
        bool write( spawned_data* m );
};

}

#endif
