#ifndef LIBCORO_CONTEXT_DETAILS_UCONTEXT_HPP
#define LIBCORO_CONTEXT_DETAILS_UCONTEXT_HPP

// Zasadniczo to nie jest jeszcze całkowicie 'uczesana' wersja
// kodu. Ale jest lepiej niż przed rozpoczęciem refaktoryzacji.
// Wystarczy dopieszczania kontekstów.

#include <exception>
#include <config.hpp>
#include <context/details/address.hpp>

#ifdef VALGRIND_TESTS_ENABLED
	#include <valgrind/valgrind.h>
#endif

namespace libcoro
{

namespace context
{

/**
 * \brief Kontekst koprocedury - implementacja używająca <tt>ucontext</tt>.
 */
namespace details
{

/**
 *\brief Publiczne składowe kontekstu - zależne od implementacji.
 */
template< typename Context = config::core::default_context, typename Stack = config::core::default_stack >
class context
{
	private: // atrybuty prywatne: brak dostępu z zewnątrz
		/// \brief Stos (implementation-specific)
		Stack core_stack;

		/// \brief Kontekst (implementation-specific)
		Context core_context;

		/// \brief klasa obsługująca adres przesyłany do make/swapcontext.
		libcoro::address adr;

	public: // aliasy typów
		typedef context< Context, Stack > * ptr;
		typedef Context * Context_ptr;
		typedef Stack * Stack_ptr;

	public: // konstruktory
		context() 
		: adr(libcoro::address(&core_context))
		{
		}

	public: // metody publiczne

		// Ciało funkcji powinno być przesłane do statycznej funkcji,
		// która będzie zaciągana z config.hpp (patrz set_parent())
		void init (libcoro::abstract::ptr ap, void(*fun)(void))
		{
			core_stack.ss_sp = new char[ libcoro::config::default_stacksize ];
#ifdef VALGRIND_TESTS_ENABLED
			VALGRIND_STACK_REGISTER(core_stack.ss_sp, (void*)((long)(core_stack.ss_sp)+libcoro::config::default_stacksize));
#endif
			core_stack.ss_size = libcoro::config::default_stacksize;
			core_stack.ss_flags = 0;
			core_context.uc_stack = core_stack;
			if ( getcontext(&core_context) == -1 ) throw std::exception();

			adr.setup_context(ap, fun);
		}

		Context_ptr get_own_context()
		{
			return &core_context;
		}

		void rewind(libcoro::abstract::ptr ap, void (*fun)(void))
		{
			adr.setup_context(ap, fun);
			//makecontext(&core_context, (void (*)())run_wrapper, 2, lo, hi);
		}

		int change(context<Context, Stack >::ptr a, context<Context, Stack >::ptr b)
		{
			return swapcontext(a->get_own_context(), b->get_own_context());
		}

		void set_parent_context(context<Context, Stack >::ptr p_context)
		{
			config::set_parent<Context>(core_context, p_context->get_own_context());
		}
};

}

}

}

#endif
