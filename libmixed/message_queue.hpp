#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <list>

namespace scheduler
{

    struct spawned_data;

    class message
    {
        public:
            typedef message* ptr;

        public:
            message( spawned_data* sp );
            message();
            ~message();
            bool used();
            void set_used();

        private:
            spawned_data* sd;
            message::ptr prev;
            message::ptr next;
            unsigned int _used: 1;
    };

    const int queue_count = 2;

    class message_queue
    {
        private:
            enum use_mode { ZEROED, READABLE, WRITEABLE };
            typedef std::list< message* > messages_t;
            messages_t mls[ queue_count ];
            use_mode ums[ queue_count ];
            int get_readable_queue();
            int get_writeable_queue();
        public:
            message_queue();
            ~message_queue();
            void clean();
            // TODO:
            bool read( message::ptr m );
            bool write( message::ptr m );
    };

}

#endif
