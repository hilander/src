#ifndef LIBCORO_FIBER_HPP
#define LIBCORO_FIBER_HPP

#include <coroutine.hpp>
#include <state_controller.hpp>

namespace scheduler
{
	class userspace_scheduler;
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
    
    void init();
    
    fiber()
    {
    }
    
    virtual ~fiber()
    {
    }
    
    virtual void go()
    {
    }
    
  public:
    virtual void run();
    
  public:
    libcoro::state_controller state;
};

}

#endif
