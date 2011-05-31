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

/** \brief Opakowane drzewo czerwono-czarne z&nbsp;powtórzeniami.
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
				current++;
				if ( current == threads.end() )
				{
					current = threads.begin();
				}
				Item* thread = 0;
				thread = ( *current ).second;
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

