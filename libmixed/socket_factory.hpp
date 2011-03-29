#ifndef SOCKET_FACTORY_HPP
#define SOCKET_FACTORY_HPP

#include <tr1/memory>

#include "poller.hpp"

namespace posix_socket
{
#include <sys/socket.h>
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
    virtual int read() = 0;

		/** \brief Zapisz (blokująco dla zapisującego włókna).
		 */
    virtual int write() = 0;

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

		/** brief oczekuj na kolejne połączenie.
		 */
    int accept();

  public:

		/** \brief Pobierz uchwyt do gniazda.
		 */
    virtual ptr get_ptr();

		/** \brief Czytaj (blokująco dla zapisującego włókna).
		 */
    virtual int read();

		/** \brief Zapisz (blokująco dla zapisującego włókna).
		 */
    virtual int write();

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

		client( int fd_
          , std::tr1::shared_ptr< posix_socket::sockaddr > sa_
          , ::socklen_t sa_len_
          , scheduler::poller::ptr p_ );

		/** \brief Pobierz uchwyt do obiektu.
		 */
    static ptr get_ptr( int fd_
                      , std::tr1::shared_ptr< posix_socket::sockaddr > sa_
                      , ::socklen_t sa_len_
                      , scheduler::poller::ptr p_ );

		/** \brief Czytaj (blokująco dla zapisującego włókna).
		 */
    virtual int read();

		/** \brief Zapisz (blokująco dla zapisującego włókna).
		 */
    virtual int write();

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

    std::tr1::shared_ptr< posix_socket::sockaddr > _sa;

    ::socklen_t _sa_len;

		scheduler::poller::ptr _p; /// \brief Epoller
};

}

}

#endif
