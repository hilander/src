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
    
  public: 
    bool send( scheduler::spawned_data& message );

		bool receive( scheduler::spawned_data& message );

		void receive_data( scheduler::spawned_data& d );

		void send_data( scheduler::spawned_data& d );
    
		// sockets (general) 
		bool read( std::vector< char >& buf, ssize_t& read_bytes, int fd_ );

		bool write( std::vector< char >& buf, ssize_t& read_bytes );
		
		// server socket
		void init_server( int fd_ );

		int accept( int fd_ );

		// client socket
		void init_client( int fd_ );

		int connect( int fd_ );

  public:
    libcoro::state_controller state;

  protected:
    scheduler::userspace_scheduler* _supervisor;

		std::list< scheduler::spawned_data > incoming_messages;
};

}

#endif
