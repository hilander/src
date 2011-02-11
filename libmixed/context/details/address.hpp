#ifndef LIBCORO_ADDRESS_HPP
#define LIBCORO_ADDRESS_HPP

#include <cstring>
#include <config.hpp>
namespace libcoro
{

class abstract;

class base_address
{
	public: // metody
		virtual void setup_context(void(*fun)(void)) = 0;

	public: // destruktor
		virtual ~base_address()
		{
		}
};

template<class T>
class address32 // : public base_address//< libcoro::abstract >
{
	public: // aliasy typów
		typedef T * c_ptr;

	public: // konstruktory
		address32(c_ptr addr)
			: c(addr)
		{
		}

	public: // metody dziedziczone z base_address'
		virtual void setup_context(libcoro::abstract* ap, void(*fun)(void))
		{
          int par[2];
          
          if ( sizeof(ap) > sizeof(int) )
          {
						memcpy( par, ap, sizeof(ap) );
            makecontext( c, fun, 2, par[0], par[1] );
          }
          else
          {
            makecontext( c, fun, 1, ap );
          }
			
		}

	public: // destruktor
		virtual ~address32()
		{
		}

	private: // atrybuty prywatne
		c_ptr c;
};

typedef address32< ::ucontext_t > address;

}

#endif
