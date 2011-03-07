#ifndef SCHEDULER_INTERFACE_HPP
#define SCHEDULER_INTERFACE_HPP

namespace scheduler
{

/** \brief Interfejs który musi implementować każdy planista.
 */
class abstract
{
  public:
    virtual void spawn( fiber::fiber::ptr fiber ) = 0;

    virtual bool send( spawned_data::ptr data ) = 0;

		virtual bool receive( spawned_data::ptr data ) = 0;

};

}

#endif
