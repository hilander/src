#ifndef SCHEDULER_CONTAINER_HPP
#define SCHEDULER_CONTAINER_HPP

#include <map>
#include <list>

namespace scheduler
{

class bad_disposal
{
};

template < typename Item, typename Container >
class container
{

	private:
		Container threads;

	public:
		Item* get();
		void dispose( Item* thread );
		bool empty();
		void insert( Item* thread );
};

/** Opakowane listy dwukierunkowe.
 */
template < typename Item >
class container < Item, std::list< Item* > >
{
	private:
		typedef std::list< Item* > list;
		typedef typename list::iterator l_it;

	private:
		list threads;
		list tmp_container;
		l_it current;

	public:
		Item* get()
		{
			Item* thread = 0;
			current = tmp_container.begin();

			if ( current != tmp_container.end() )
			{
				thread = *( current );
			}

			return thread;
		}

		void block(container< Item, std::list< Item* > > c) 
		{
			if ( tmp_container.begin() != tmp_container.end() )
			{
				threads.splice( c.end(), tmp_container, tmp_container.begin() );
			}
		}

		void suspend()
		{
			threads.splice( threads.end(), threads, current );
		}

		void dispose()
		{
			threads.erase( current );
		}

		void insert( Item* thread )
		{
			threads.push_front( thread );
		}

		bool empty()
		{
			return threads.empty();
		}

		int size()
		{
			return threads.size();
		}
};

/** \brief Opakowane drzewo czerwono-czarne z&nbsp;powtórzeniami.
 *
 * Jako klucz stosujemy czas działania.
 */
template < typename Item >
class container < Item, std::map< Item*, Item* > >
{
	private:
		typedef std::map< Item*, Item* > map;
		typedef typename map::iterator m_it;

	private:
		map threads;
		m_it current;
		::clock_t start_tick;

	public:
	container()
	{
		current = threads.begin();
	}
		Item* get()
		{
			Item* thread = 0;
			current = threads.begin();

			if ( current != threads.end() )
			{
				thread = ( *current ).second;
			}
			return thread;
		}

		void suspend()
		{
			//threads.insert( std::pair< Item*, Item* >( ( *current ).first, ( *current ).second ) );
			//( *current ).second = 0;
			//threads.erase( current );
		}

		void dispose()
		{
			m_it tmp = current;
			(*current).second = 0;
			threads.erase( current );
		}

		void insert( Item* thread )
		{
			threads.insert( std::pair< Item*, Item* >( thread, thread ) );
		}

		bool empty()
		{
			return threads.empty();
		}

		int size()
		{
			return threads.size();
		}
};

}

#endif
