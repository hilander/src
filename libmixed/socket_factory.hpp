#ifndef SOCKET_FACTORY_HPP
#define SOCKET_FACTORY_HPP

namespace scheduler
{

namespace sockets
{

class tcp_server
{
  public:
    typedef std::tr1::shared_ptr< tcp_server > ptr;

  public:
    ptr get_ptr();
    int accept();
    int read();
    int write();
};

}

}

#endif
