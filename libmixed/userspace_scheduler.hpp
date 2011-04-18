#ifndef USERSPACE_SCHEDULER_HPP
#define USERSPACE_SCHEDULER_HPP

#include <string.h>
#include <errno.h>
#include <factory.hpp>
#include <list>
#include <iostream>
#include <container.hpp>
#include <scheduler_tools.hpp>
#include <manager.hpp>
#include <scheduler_interface.hpp>

namespace scheduler
{

class ueber_scheduler;

/** \brief Planista S_ULT.
 *
 * Klasa zarządcy wątków przestrzeni użytkownika.
 */
class userspace_scheduler : public libcoro::coroutine, public abstract
{
	public:
		typedef userspace_scheduler* ptr;
		typedef std::list< userspace_scheduler::ptr > list;

	public:
		/** \brief Konstruktor.
		 */
		userspace_scheduler(ueber_scheduler* ptr);

		/** \brief Konstruktor - dla celów debugowania.
		 */
        userspace_scheduler(ueber_scheduler* ptr, std::list< fiber::fiber* > fibers_ );

	public:
		/** \brief Inicjalizacja planisty.
		 */
		virtual void init( raw_pipe::ptr message_pipe );

		/** \brief Entry point dla wątku planisty.
		 */
		static void* go( void* data );

		/** \brief Zaczekaj na zakończenie wątku.
         * Obecnie czeka na zakończenie planisty.
		 */
		void join( fiber::fiber::ptr f );

		/** \brief Czy można zakończyć działanie planisty?
		 */
		virtual bool finished();

		/** \brief Zakończ działanie planisty.
		 */
		virtual void finish();

		/** 
         * \brief <i>Entry Point</i> planisty.
		 */
		virtual void run();

		/** \brief Rozpocznij działanie planisty.
		 */
		virtual void start();

		/** \brief Czy posiadamy jakieś wątki, które należy uruchomić?
		 */
		virtual bool empty();

		/** \brief Bieżące obciążenie planisty.
		 */
		int get_workload();

		/** \brief Zablokuj wątek.
		 */
		void block( scheduler::data_kind k, fiber::fiber::ptr f, void* data_ );

		void block( scheduler::data_kind k, fiber::fiber::ptr f, int fd_ );

		void read_messages();

		bool ended();

  public: // metody wymagane przez interfejs scheduler::abstract

    /** \brief Utwórz wątek i&nbsp;oddaj go w&nbsp;opiekę planiście.
     * Ta metoda wysyła żądanie do uls.
     */
    virtual void spawn( fiber::fiber::ptr fiber );

		/** \brief Utwórz wątek (uls side).
		 * Ta metoda nie może byc wołana z zewnątrz uls-a, użyj spawn(fiber::fiber::ptr fiber).
		 */
		void spawn(void* f, bool confirm);

    virtual bool send( spawned_data& data );

		virtual bool receive( spawned_data& data );

		//sockets
		virtual bool read( std::vector< char >& buf_ , ssize_t& read_bytes_, fiber::fiber::ptr caller, int fd_ );

		virtual bool write( std::vector< char >& buf_ , ssize_t& read_bytes_, fiber::fiber::ptr caller, int fd_ );

		virtual bool init_server( int fd_, fiber::fiber::ptr caller );

		virtual int accept( int fd_, fiber::fiber::ptr caller, accept_connect_data::ptr data );

		virtual bool init_client( int fd_, fiber::fiber::ptr caller );

		virtual bool connect( int fd_, fiber::fiber::ptr caller, accept_connect_data::ptr data );

    virtual bool close( int fd_, fiber::fiber::ptr caller );

	private:
		libcoro::coroutine::ptr base_coroutine;
		libmanager::manager::ptr manager;

		thread_container ready;
		std::map< int, scheduler::data_kind* > socket_descriptors;
		std::map< int, int > waiting_descriptors;

		bool scheduler_end;
		int workload;
		ueber_scheduler* us;
		raw_pipe::ptr message_device;

		::pthread_t uls_thread;
		::pthread_attr_t uls_attr;
		bool _ended;
};

}

#endif
