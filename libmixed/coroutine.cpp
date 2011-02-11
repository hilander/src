#include <coroutine.hpp>
#include <context/context.hpp>
#include <context/details/ucontext.hpp>

using namespace libcoro;

coroutine::coroutine()
{
}

void
coroutine::init(context::context< config::core::default_context >* c)
{
	ctx = c;
}

void
coroutine::start(coroutine::ptr return_to)
{
	return_coroutine = return_to;
	ctx->switch_to(return_to->ctx);
}

void
coroutine::rewind()
{
	ctx->rewind(this, (void(*)()) run_wrapper);
}

void
coroutine::yield()
{
	yield( false );
}

void
coroutine::yield(bool blocked)
{
	ctx->return_to(return_coroutine->ctx);
}

void
coroutine::exit()
{
	ctx->return_to(return_coroutine->ctx);
}

void
libcoro::run_wrapper(coroutine::ptr c)
{
	c->wrap_run();
	// stare rozwiązanie - nie polityczne ani trochę,
	// gdyż powinno być realizowane również w obiekcie klasy <tt>coroutine</tt>
	c->ctx->return_to(c->return_coroutine->ctx);
}
