#ifndef LIBCORO_FIBER_HPP
#define LIBCORO_FIBER_HPP

#include <coroutine.hpp>
#include <state_controller.hpp>
#include <scheduler_tools.hpp>

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

    void set_supervisor( scheduler::abstract* supervisor_ );
    
  public: 
    bool send( scheduler::spawned_data*& message );

		bool receive( scheduler::spawned_data*& message );

		void receive_data( scheduler::spawned_data*& d );

		void send_data( scheduler::spawned_data*& d );
    
  public:
    libcoro::state_controller state;

  protected:
    scheduler::abstract* _supervisor;

		std::list< scheduler::spawned_data* > incoming_messages;
};

}

#endif
