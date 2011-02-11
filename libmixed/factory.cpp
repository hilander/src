#include <factory.hpp>

using namespace libcoro;
using namespace config;


coroutine::ptr 
factory::create_coroutine()
{
  libcoro::coroutine::ptr c = new libcoro::coroutine();
  
  config::context::ptr ctx = new config::context::ref();
  ctx->init(c, (void(*)()) libcoro::run_wrapper);
  c->init(ctx);
  return c;
}

void
factory::assign_coroutine(coroutine::ptr c)
{
  config::context::ptr ctx = new config::context::ref();
  ctx->init(c, (void(*)()) &(libcoro::run_wrapper));
  c->init(ctx);
}

void 
factory::assign_coroutine( coroutine::ptr c, config::context::ptr ctx )
{
  ctx->init(c, (void(*)()) &(libcoro::run_wrapper));
  c->init(ctx);
}