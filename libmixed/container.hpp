#ifndef SCHEDULER_CONTAINER_HPP
#define SCHEDULER_CONTAINER_HPP

#include <map>
#include <list>
#include <iostream>

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
		void erase( Item* elem );
    bool exists( Item* elem );
};

/** Opakowane listy dwukierunkowe.
 * Chwilowo nieczynne :)
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

    bool exists( Item* elem )
    {
      for ( l_it i = threads.begin();
            i != threads.end();
            i++ )
      {
        if ( (*i) == elem )
        {
          return true;
        }
      }

      return false;
    }
};
 * _end_ class container < Item, std::list< Item* > > */

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

	public:
    container()
    {
			current = threads.begin();
    }

		Item* get()
		{
			if( threads.size() == 0 )
			{
				return 0;
			}
			else
			{
				//std::cout << "container<map>::get(): threads.sise() = " << threads.size() << std::endl;
				Item* thread = 0;
				thread = ( *current ).second;
				current++;
				if ( current == threads.end() )
				{
					current = threads.begin();
					//std::cout << "container<map>::get(): current <- threads.begin()" << std::endl;
				}
				return thread;
			}
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
			(*tmp).second = 0;
			threads.erase( tmp );
			current = threads.begin();
		}

		void erase( Item* elem )
		{
			m_it found = threads.find( elem );

			if ( found != threads.end() )
			{
				if ( found == current ) // should be...
				{
					++current;
					if ( current == threads.end() )
					{
						current = threads.begin();
					}
				}
				threads.erase( found );
			}
		}

		void insert( Item* thread )
		{
			threads.insert( std::pair< Item*, Item* >( thread, thread ) );

			//mini-reset:
			current = threads.begin();
		}

		bool empty()
		{
			return threads.empty();
		}

		int size()
		{
			return threads.size();
		}
    
    bool exists( Item* elem )
    {
      if ( threads.find( elem ) != threads.end() )
      {
        return true;
      }
      else
      {
        return false;
      }
    }
};

}

#endif
