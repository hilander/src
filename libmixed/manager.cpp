#include <coroutine.hpp>
#include <factory.hpp>
#include "manager.hpp"

using namespace libmanager;

manager::manager()
{
}

//runnable::runnable::ptr
void
manager::get( fiber::fiber::ptr f )
{
		libcoro::factory::assign_coroutine( f );
		/* todo w innym życiu...
	if ( ready.empty() )
	{
		libcoro::factory::assign_coroutine( f );
	}
	else
	{
		container::iterator it;
		it = ready.begin();
		libcoro::factory::assign_coroutine( f, *( it ) );
		used.splice( it, ready, it );
	}
	*/
}

void
manager::release( fiber::fiber::ptr r )
{
	mgr_container::iterator i_front, i_back;

	i_back = used.end();
	for ( i_front = used.begin() ; i_front != i_back; i_front++ )
	{
		i_back--;
		if ( *(i_back) == r->ctx )
		{
			ready.splice( i_back , used, i_back );
			break;
		}

		if ( *(i_front) == r->ctx )
		{
			ready.splice( i_front, used, i_front );
			break;
		}
	}
}
