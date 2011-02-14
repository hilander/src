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
  message();
  ~message();
  bool used();

  private:
  spawned_data* sd;
  message::ptr prev;
  message::ptr next;
  unsigned int _used: 1;
};

class message_queue
{
  private:
    std::list< message > ml;
  public:
    message_queue();
    ~message_queue();
    void clean();
    // TODO:
    //read();
    //write();
};

}

#endif
