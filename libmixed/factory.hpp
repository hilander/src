#ifndef LIBCORO_COROUTINE_FACTORY_HPP
#define LIBCORO_COROUTINE_FACTORY_HPP

#include <coroutine.hpp>
#include <context/context.hpp>
#include <context/details/ucontext.hpp>
#include <config.hpp>

namespace libcoro
{

namespace config
{

struct context
{
    typedef libcoro::context::context < ::ucontext_t > ref;
    typedef libcoro::context::context < ::ucontext_t > * ptr;
};

}

/** \brief Fabryka koprocedur.
 * Fabrykę tworzą funkcje, które wytwarzają koprocedury.<br />
 * Dodatkowo, w fabryce znajduje się <i>warsztat</i>, który 
 * wymienia składowe koprocedur.
 */
namespace factory
{
		/*
		 * \brief Utwórz koprocedurę oraz jej kontekst.
		 */
		coroutine::ptr create_coroutine();

		/*
		 * \brief dla podanej koprocedury, utwórz i zwiąż z nią odpowiedni kontekst.
		 */
		void assign_coroutine(coroutine::ptr c);

		/*
		 * \brief dla podanej koprocedury, zwiąż z nią odpowiedni kontekst.
		 */
		void assign_coroutine( coroutine::ptr c, config::context::ptr ctx );
}

}

#endif
