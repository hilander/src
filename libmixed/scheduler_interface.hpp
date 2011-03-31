#ifndef SCHEDULER_INTERFACE_HPP
#define SCHEDULER_INTERFACE_HPP

#include <vector>

namespace scheduler
{

/** \brief Interfejs który musi implementować każdy planista.
 */
class abstract
{
  public:
    virtual void spawn( fiber::fiber::ptr fiber ) = 0;

    virtual bool send( spawned_data& data ) = 0;

		virtual bool receive( spawned_data& data ) = 0;
};

}

#endif
