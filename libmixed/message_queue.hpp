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
    public:
        typedef std::list< spawned_data* > messages_t;

    private:
        messages_t _messages;
        pthread_mutex_t _mutex;
        pthread_mutexattr_t _mattrs;

    public:
        message_queue();
        ~message_queue();
        void clean();

        bool read( spawned_data* m );
        bool write( spawned_data* m );
};

}

#endif
