#include <libc-base.hpp>

namespace pl2d::cache {

dlimport auto alloc(usize size) -> void *ownership_returns(pl2d_alloc);

dlexport void free(void *ptr) ownership_takes(pl2d_alloc, 1);

} // namespace pl2d::cache
