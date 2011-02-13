#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

namespace scheduler
{

class message
{
};

class message_queue
{
  private:
    std::list< message > ml;
  public:
    message_queue();
    ~message_queue();
    void clean();
};

}

#endif
