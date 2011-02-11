#include <config.hpp>

template <>
void 
libcoro::config::set_parent< ::ucontext_t >(::ucontext_t * ctx, ::ucontext_t * parent_ctx)
{
	ctx->uc_link = parent_ctx;
}

