#ifndef LIBCORO_STATE_CONTROLLER_HPP
#define LIBCORO_STATE_CONTROLLER_HPP

namespace libcoro
{

class state_controller
{
	public: // typy
		/// \brief Stan koprocedury.
		enum state
		{
			/// \brief Koprocedura zainicjalizowana poprawnie, gotowa do (ponownego) uruchomienia.
			READY,

			/// \brief Koprocedura w trakcie działania.
			RUNNING, 

			/// \brief Funkcja <tt>run</tt> zakończyła działanie.
			FINISHED, 

			/// \brief Koprocedura zablokowana (np. oczekuje na I/O).
			BLOCKED, 

			/// \brief Wyjątek w trakcie działania koprocedury; wykonywanie koprocedury przerwane.
			EXCEPTION, 

			/// \brief Błąd w trakcie działania koprocedury; wykonywanie koprocedury przerwane.
			ERROR, 

			/// \brief Nieokreślony stan koprocedury.
			UNDEFINED 
		};

		//public: // aliasy typów

	public: // metody

		/// \brief Ustawia środowisko podczas blokowania koprocedury.
		virtual void block() { s = BLOCKED; }

		/// \brief Ustawia środowisko po odblokowaniu koprocedury.
		virtual void unblock() { s = READY; }

		/// \brief Ustawia środowisko po iniclaizacji koprocedury.
		virtual void on_init() { s = READY; }

		/// \brief Ustawia środowisko przed uruchomieniem koprocedury.
		virtual void on_start() { s = RUNNING; }

		/// \brief Ustawia środowisko po zwróceniu częściowego wyniku.
		virtual void on_yield() { s = READY; }

		/// \brief Ustawia środowisko po zakończeniu koprocedury.
		virtual void on_exit() { s = FINISHED; }

		/// \brief Ustawia środowisko po wystąpieniu błędu w wykonaniu koprocedury.
		virtual void on_error() { s = ERROR; }

		/// \brief Ustawia środowisko po wystąpieniu wyjątku w wykonaniu koprocedury.
		virtual void on_exception() { s = EXCEPTION; }

		/// \brief Aktualny stan koprocedury.
		virtual state get() { return s; }

	private: // atrybuty prywatne

		/// \brief Stan koprocedury.
		state s;
};

}

#endif
