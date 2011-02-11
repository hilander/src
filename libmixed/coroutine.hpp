#ifndef LIBCORO_COROUTINE_HPP
#define LIBCORO_COROUTINE_HPP

#include <exception>
#include <config.hpp>
#include <context/details/ucontext.hpp>
#include <context/context.hpp>

/**
 * \brief Widoczna dla użytkownika część biblioteki.
 * W przestrzeni nazw <tt>libcoro</tt> znajdują się
 * deklaracje klas oraz funkcje,
 * które mogą być używane przez użytkownika.
 * Deklaracje bytów bezpośrednio związanych z&nbsp;konkretną
 * implementacją winny znajdować się w&nbsp;wewnętrznych
 * podprzestrzeniach nazw i&nbsp;nie powinny
 * <i>zaprzątać uwagi</i> użytkownika.
 */
namespace libcoro
{

/**
 * \brief Podstawowa klasa koprocedur.
 */
class coroutine : public abstract
{
	public: // aliasy typów
		typedef coroutine * ptr;

	protected: // attributes
		/// \brief Kontekst koprocedury.
		//config::context::ptr context;
	
	public: // attributes

		/// \brief Kontekst koprocedury.
		context::context< config::core::default_context >* ctx;

		/// \brief Koprocedura, do której domyślnie powróci sterowanie po zwróceniu wyniku (również częściowego).
		coroutine::ptr return_coroutine;

	private: // attributes

	public: // konstruktory
		/** \brief Konstruktor koprocedury.
		 * Po utworzeniu, koprocedura <b>nie jest gotowa do uruchomienia</b>.
		 * Należy ją jeszcze zainicjalizować funkcją <tt>init()</tt>.
		 * \see init()
		 */
		coroutine();

	public: // methods
		/** Przygotowanie koprocedury do działania.
		 * \param ret_coroutine - koprocedura (domyślna), do której wróci sterowanie po wyjściu z bieżącej koprocedury.
		 */
		virtual void init(context::context< config::core::default_context >* c);

		/** 
		 * \brief Uruchomienie koprocedury.
		 * Należy zdecydować, jak będzie się nazywać metoda startująca
		 * \see start()
		 */
		virtual void run(){} // entry point koprocedury

		/** 
		 * \brief Uruchomienie koprocedury.
		 * Należy zdecydować, jak będzie się nazywać metoda startująca
		 * \see start()
		 */
		void start(coroutine::ptr return_to); // uruchomienie koprocedury; może nawet redundantne dla resume()

		/** \brief Przewiń wątek do początku.
		 * Potrzebna metoda: Jeśli mamy jakąś koprocedurę w magazynie,
		 * nie musimy jej wyrzucać. Wystarczy przewinąć ją do początku i odświeżyć.
		 */
		void rewind(); // wznowienie działania koprocedury

		/**
		 * \brief Zablokuj koprocedurę w oczekiwaniu na zdarzenie.
		 */
		void yield (bool blocked); // zwróć wynik częściowy

		/**
		 * \brief Zwróć częściowy wynik oraz przekaż sterowanie.
		 */
		void yield (); // zwróć wynik częściowy

		/**
		 * \brief Zakończ działanie koprocedury.
		 */
		void exit ();

		/**
		 * \brief Wywołanie ciała koprocedury w jej własnym kontekście.
		 * Wyłapywanie wyjątków powinno być relizowane przez użytkowika,
		 * dlatego wystąpiła potrzeba stworzenia tej metody tu i teraz ;)
		 */
		virtual void wrap_run()
		{
			try
			{
				run();
                // wywalanie odwołań do stanów
				//state.on_exit();
				exit();
			}
			catch (std::exception & e)
			{
				// Umiemy złapać wyjątek standardowy! Czyli możemy
				// z nim zrobić, cokolwiek chcemy. Np. nic :)
                // wywalanie odwołań do stanów
				//state.on_exception();
			}
			catch (...)
			{
				// Wystąpił wyjątek nie przewidziany przez użytkownika.
				// Czyli możemy potraktować go jako błąd.
                // wywalanie odwołań do stanów
				//state.on_error();
			}
		}

		//void set_state_controller(state_controller& sc)
		//{
		//	state = sc;
		//}
};

void run_wrapper(coroutine::ptr c); // funkcja, która będzie rozpoczynać wykonanie koprocedury

}

#endif
