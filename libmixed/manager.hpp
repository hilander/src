#ifndef LIBCORO_MANAGER_HPP
#define LIBCORO_MANAGER_HPP

#include <coroutine.hpp>
#include <list>
#include "fiber.hpp"
#include "config.hpp"

/** \brief Przestrzeń nazw zarządcy stosów.
 */
namespace libmanager
{


/// Mechanizm!
/// W zależności od stosowanej polityki, możemy chcieć sobie
/// użyć czegoś bardziej wysublimowanego niż lista dwukierunkowa.
typedef std::list< libcoro::config::context::ptr > mgr_container;

class manager : public libcoro::coroutine
{
	public:
		typedef manager* ptr;

	public: // konstruktory publiczne
		manager();

	public: // metody publiczne
		virtual void init()
		{}

		void get( fiber::fiber::ptr r );
		void release(fiber::fiber::ptr r);
	private:
		/// \brief Kontener zawiera aktualnie używane włókna.
		mgr_container used;

		/// \brief Kontener zawiera użyte włókna, które mogą być użyte ponownie.
		mgr_container ready;

};

}

#endif
