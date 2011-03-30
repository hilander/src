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

    virtual bool send( spawned_data::ptr data ) = 0;

		virtual bool receive( spawned_data::ptr data ) = 0;

		//sockets
		virtual bool read( std::vector< char >& buf_ , ssize_t& read_bytes_ ) = 0;

		virtual bool write( std::vector< char >& buf_ , ssize_t& read_bytes_ ) = 0;

		virtual void init_server( int fd_ ) = 0;

		virtual int accept( int fd_ ) = 0;

		virtual void init_client( int fd_ ) = 0;

		virtual int connect( int fd_ ) = 0;
};

}

#endif
