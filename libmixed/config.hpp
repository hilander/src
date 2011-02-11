#ifndef LIBCORO_CONFIG_HPP
#define LIBCORO_CONFIG_HPP

#include <ucontext.h>

namespace libcoro
{

class abstract
{
	public: // aliasy typów
		typedef abstract * ptr;

	public: // metody
		/** 
		 * \brief Uruchomienie koprocedury.
		 * Należy zdecydować, jak będzie się nazywać metoda startująca
		 * \see start()
		 */
		virtual void run() = 0; // entry point koprocedury
};

namespace config
{

/**
 * \brief Domyślny rozmiar stosu koprocedury.
 */
const int default_stacksize = 8192;


struct core
{
	typedef ::ucontext_t default_context;
	typedef ::stack_t default_stack;
};

template < typename CoreContext >
void set_parent(CoreContext * ctx, CoreContext * parent_ctx);

template <>
void set_parent< ::ucontext_t >(::ucontext_t * ctx, ::ucontext_t * parent_ctx);

}

}

#endif
