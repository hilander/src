#ifndef SCHEDULER_SCHEDULER_HPP
#define SCHEDULER_SCHEDULER_HPP

#include <factory.hpp>
#include <list>
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <queue>
#include <vector>

#include <manager.hpp>
#include <fiber.hpp>
#include <container.hpp>
#include <scheduler_tools.hpp>
#include <userspace_scheduler.hpp>
#include <scheduler_interface.hpp>
#include "poller.hpp"

/** \brief Przestrzeń nazw planisty wątków.
 *
 * W&nbsp;tej przestrzeni znajdują się dwie klasy planistów wątków:
 *  - planista dla ULT (S_ULT);
 *  - planista zarządzający planistami S_ULT (U_SCH).
 *
 * Planista S_ULT zarządza grupą wątków, jakie zostały mu przekazane.
 * Wykonuje je w&nbsp;ramach jednego wątku.
 *
 * Planista U_SCH:
 *  - wytwarza nowe S_ULT;
 *  - <i>zaleca</i> zakończenie ich działania;
 *  - przydziela wątki poszczególnym S_ULT;
 *  - przekazuje wątki pomiędzy S_ULT.
 *
 * <b>TODO:</b>
 *
 *  - rozsyła komunikaty do odpowiednich S_ULT. 
 */
namespace scheduler
{

/** \brief Planista U_SCH.
 */
class ueber_scheduler : public libcoro::coroutine, public abstract
{
	public:
		ueber_scheduler();

    ~ueber_scheduler();
    
	public:
		static void* go ( void* obj );

		/** \brief Inicjalizacja nad-zarządcy
		 * <ul>
		 * <li> uruchomienie maszyny odpowiedzialnej za zarządzanie wątkami KLT;
		 * <li> utworzenie zarządców ULT.
		 * </ul>
		 */
		virtual void init( std::list< userspace_scheduler* >* local_schedulers = 0 );

		virtual bool finished();

		virtual void finish();

		virtual void run();

		bool get_from_pipe(raw_pipe* rp, spawned_data& sp);

		bool read_and_interpret ( raw_pipe* rp, spawned_data& pc );

		virtual void start();

    void join_u_sch();

    libmanager::manager::ptr get_manager();

    void move_to_blocked( int fd_, scheduler::spawned_data& f_ );

    void delete_from_blocked( int fd_ );

  public: // Interfejs abstract:
		virtual void spawn( fiber::fiber::ptr fiber );

    virtual bool send( spawned_data& data );

		virtual bool receive( spawned_data& data );

	private:
		void create_local_schedulers( std::list< userspace_scheduler* >* list_ );

		void create_local_schedulers();

		void do_epolls();

		void do_connect( spawned_data& orig_mess );

		void set_epoll_response( ::epoll_event& e, spawned_data& resp, spawned_data& orig_mess );

		static void* stub_go( void* obj );

		::pthread_barrier_t barrier;

		libcoro::coroutine::ptr base_coroutine;
		libmanager::manager manager;

		::pthread_attr_t stack_attr;
		std::map< int, scheduler::spawned_data > blocked;
		::pthread_t main_thread;
		::pthread_mutex_t container_is_ready;

		::pthread_mutex_t finish_check_lock;
		bool scheduler_end;
		int blocked_num;

		userspace_scheduler::list schedulers;
		libmanager::manager man;
		std::list<raw_pipe*> pipes;
		poller::ptr epoller;
};

}

#endif
