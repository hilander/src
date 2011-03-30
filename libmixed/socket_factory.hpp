#ifndef SOCKET_FACTORY_HPP
#define SOCKET_FACTORY_HPP

#include <tr1/memory>
#include <vector>

#include "poller.hpp"

namespace posix
{
#include <sys/socket.h>
#include <unistd.h>
}

namespace scheduler
{

namespace sockets
{

class abstract
{
  public:
    typedef abstract* ptr;

  public:

		/** \brief Czytaj (blokująco dla zapisującego włókna).
		 */
    virtual int read( std::vector< char >& buf ) = 0;

		/** \brief Zapisz (blokująco dla zapisującego włókna).
		 */
    virtual int write( std::vector< char >& buf ) = 0;

		/** Zainicjalizuj obiekt
		 */
		virtual void init() = 0;

	protected:

		/** \brief Zarejestruj gniazdo.
		 */
		virtual bool do_register( int fd_ ) = 0;

		/** \brief Wyrejestruj gniazdo.
		 */
		virtual void deregister( int fd_ ) = 0;
};

class server : public abstract
{
  public:
    typedef server* ptr;

	public:

		/** \brief Oczekuj na kolejne połączenie.
		 */
    int accept();

		/** \brief usuń gniazdko dla połączenia.
		 */
		 void cancel( int fd_ );

  public:

		/** \brief ctor
		 */
		server( int fd_
          , std::tr1::shared_ptr< posix::sockaddr > sa_
          , ::socklen_t sa_len_
          , scheduler::poller::ptr p_ );

		/** \brief dtor
		 */
		~server();

		/** \brief Pobierz uchwyt do gniazda.
		 */
    static ptr get_ptr( int fd_
                      , std::tr1::shared_ptr< posix::sockaddr > sa_
                      , ::socklen_t sa_len_
                      , scheduler::poller::ptr p_ );

		/** \brief Czytaj (blokująco dla zapisującego włókna).
		 */
    virtual int read( std::vector< char >& buf );

		/** \brief Zapisz (blokująco dla zapisującego włókna).
		 */
    virtual int write( std::vector< char >& buf );

		/** Zainicjalizuj obiekt
		 */
		virtual void init();

	protected:

		/** \brief Zarejestruj gniazdo.
		 */
		virtual bool do_register( int fd_ );

		/** \brief Wyrejestruj gniazdo.
		 */
		virtual void deregister( int fd_ );

	private:

		server() {} /// nie używać

		int _fd;

    std::tr1::shared_ptr< posix::sockaddr > _sa;

    ::socklen_t _sa_len;

		scheduler::poller::ptr _p; /// \brief Epoller
};

class client : public abstract
{
  public:
    typedef client* ptr;

	public:

		/** \brief Spróbuj połączyć się z serwerem
		 */
    int connect();

  public:

		/** \brief ctor
		 */
		client( int fd_
          , std::tr1::shared_ptr< posix::sockaddr > sa_
          , ::socklen_t sa_len_
          , scheduler::poller::ptr p_ );

		/** \brief dtor
		 */
		~client();

		/** \brief Pobierz uchwyt do obiektu.
		 */
    static ptr get_ptr( int fd_
                      , std::tr1::shared_ptr< posix::sockaddr > sa_
                      , ::socklen_t sa_len_
                      , scheduler::poller::ptr p_ );

		/** \brief Czytaj (blokująco dla zapisującego włókna).
		 */
    virtual int read( std::vector< char >& buf );

		/** \brief Zapisz (blokująco dla zapisującego włókna).
		 */
    virtual int write( std::vector< char >& buf );

		/** Zainicjalizuj obiekt
		 */
		virtual void init();

	protected:

		/** \brief Zarejestruj gniazdo.
		 */
		virtual bool do_register( int fd_ );

		/** \brief Wyrejestruj gniazdo.
		 */
		virtual void deregister( int fd_ );

	private:

		client() {} // nie używać

	private:

		int _fd;

    std::tr1::shared_ptr< posix::sockaddr > _sa;

    ::socklen_t _sa_len;

		scheduler::poller::ptr _p; /// \brief Epoller
};

}

}

#endif
