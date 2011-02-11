#ifndef LIBCORO_CONTEXT_HPP
#define LIBCORO_CONTEXT_HPP

namespace libcoro
{

/**
 * \brief Kontekst koprocedury.
 */
namespace context
{

/**
 * \brief Kontekst koprocedury.
 */
template <class T>
class context
{
	private: // pola
	  details::context< T > internals;

	public: // konstruktor
		context()
			:internals()
		{
		}

	public: // aliasy typów
		typedef context< T > * ptr;
		
	public: // metody
		void init(libcoro::abstract::ptr cp, void (*fun)(void))
		{
			internals.init(cp, fun);
		}

		void rewind(libcoro::abstract::ptr c, void (*fun)(void))
		{
			internals.rewind(c, fun);
		}

		int switch_to( context::ptr to )
		{
			return internals.change( &to->internals, &internals );
		}

		int return_to( context::ptr to )
		{
			return internals.change( &internals, &to->internals );
		}

};

}

}
#endif
