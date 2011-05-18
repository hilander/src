#ifndef LIBCORO_FIBER_HPP
#define LIBCORO_FIBER_HPP

#include <vector>

#include "coroutine.hpp"
#include "state_controller.hpp"
#include "scheduler_tools.hpp"

namespace scheduler
{
  class abstract;
	class userspace_scheduler;
  struct spawned_data;
	struct accept_connect_data;
}

/** \brief Przestrzeń nazw klas podstawowych dla wątków.
 */
namespace fiber
{


class fiber : public libcoro::coroutine
{
  public:
    typedef fiber * ptr;
    typedef ptr handle;
    
  public:
    
    fiber();
    
    virtual ~fiber();
    
    void init();

    virtual void go() = 0;
    
    virtual void run();

    void set_supervisor( scheduler::userspace_scheduler* supervisor_ );

		void wait();
    
  public: 

    void spawn( fiber::fiber::ptr f );

    bool send( scheduler::spawned_data& message );

		bool receive( scheduler::spawned_data& message );

		void receive_data( scheduler::spawned_data& d );

		void send_data( scheduler::spawned_data& d );

		void create_fiber( fiber::ptr fp_ );
    
		// sockets (general) 
		bool read( void* buf, ssize_t& read_bytes, int fd_ );

		bool write( void* buf, ssize_t& read_bytes, int fd_ );
		
		// server socket
		bool accept( int fd_, scheduler::accept_connect_data& data_ );

		// client socket
		bool connect( int fd_, scheduler::accept_connect_data& data_ );

    void do_close( int fd_ );

  public:
    libcoro::state_controller state;

  protected:
    scheduler::userspace_scheduler* _supervisor;

		std::list< scheduler::spawned_data > incoming_messages;

		std::list< int > connections;
};

}

#endif
