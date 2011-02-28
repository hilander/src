#ifndef USERSPACE_SCHEDULER_HPP
#define USERSPACE_SCHEDULER_HPP

#include <string.h>
#include <errno.h>
#include <factory.hpp>
#include <list>
#include <iostream>
#include "container.hpp"
#include "scheduler_tools.hpp"
#include "manager.hpp"

namespace scheduler
{

class abstract;
class ueber_scheduler;
/** \brief Planista S_ULT.
 *
 * Klasa zarządcy wątków przestrzeni użytkownika.
 */
class userspace_scheduler : public libcoro::coroutine
{
	public:
		typedef userspace_scheduler* ptr;
		typedef std::list< userspace_scheduler::ptr > list;

	public:
		/** \brief Konstruktor.
		 */
		userspace_scheduler(ueber_scheduler* ptr);

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

		/** \brief Utwórz wątek i&nbsp;oddaj go w&nbsp;opiekę planiście.
		 * Ta metoda jest wystawiona dla użytkownika: wysyła żądanie do uls.
		 */
		virtual void spawn( fiber::fiber::ptr fiber );

		/** \brief Utwórz wątek (uls side).
		 * Ta metoda nie może byc wołana z zewnątrz uls-a, użyj spawn(fiber::fiber::ptr fiber).
		 */
		void spawn(void* f, int);

		/** \brief Czy posiadamy jakieś wątki, które należy uruchomić?
		 */
		virtual bool empty();

		/** \brief Bieżące obciążenie planisty.
		 */
		int get_workload();

		/** \brief Zablokuj wątek.
		 */
		void block( fiber::fiber::ptr f );

		void send_message( data_kind k, void* d );

		void read_messages();

	private:
		libcoro::coroutine::ptr base_coroutine;
		libmanager::manager::ptr manager;

		thread_container ready;
		bool scheduler_end;
		int workload;
		ueber_scheduler* us;
		raw_pipe::ptr message_device;

		::pthread_t uls_thread;
		::pthread_attr_t uls_attr;
};

}

#endif
