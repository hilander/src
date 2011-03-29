#ifndef SOCKET_FACTORY_HPP
#define SOCKET_FACTORY_HPP

#include "poller.hpp"

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

	protected:

		/** \brief Zarejestruj gniazdo.
		 */
		virtual bool do_register( int fd_ ) = 0;

		/** \brief Wyrejestruj gniazdo.
		 */
		virtual void deregister( int fd_ ) = 0;

		/** Zainicjalizuj obiekt
		 */
		virtual void init() = 0;
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

	protected:

		/** \brief Zarejestruj gniazdo.
		 */
		virtual bool do_register( int fd_ );

		/** \brief Wyrejestruj gniazdo.
		 */
		virtual void deregister( int fd_ );

		/** Zainicjalizuj obiekt
		 */
		virtual void init();
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

		/** \brief Pobierz uchwyt do obiektu.
		 */
    static ptr get_ptr();

		/** \brief Czytaj (blokująco dla zapisującego włókna).
		 */
    virtual int read();

		/** \brief Zapisz (blokująco dla zapisującego włókna).
		 */
    virtual int write();

	protected:

		/** \brief Zarejestruj gniazdo.
		 */
		virtual bool do_register( int fd_ = _fd);

		/** \brief Wyrejestruj gniazdo.
		 */
		virtual void deregister( int fd_ = _fd );

		/** Zainicjalizuj obiekt
		 */
		virtual void init();

	private:

		client() {} // nie używać

		client( scheduler::poller::ptr p_ );

	private:

		scheduler::poller::ptr _p; /// \brief Epoller

		int _fd;
};

}

}

#endif
