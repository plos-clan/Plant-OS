#include <pl2d.hpp>

namespace pl2d::cache {

static constexpr usize CACHE_TOTAL_SIZE  = 2_MiB;
static constexpr usize CACHE_MAXBLK_SIZE = 512_KiB;
static constexpr usize CACHE_MINBLK_SIZE = 512_B;

static constexpr usize NBLOCKS = cpp::fhsb(CACHE_MAXBLK_SIZE / CACHE_MINBLK_SIZE) + 1;

static val cache_addr = ::malloc(CACHE_TOTAL_SIZE);

static bool caches_alloced[NBLOCKS][2] = {};

finline auto blk_id_by_size(usize size) -> int {
  assume(size > 0);
  val id = cpp::fhsb(size - 1) - cpp::fhsb(CACHE_MINBLK_SIZE) + 1;
  return id < 0 ? 0 : id;
}

finline auto blk_id_by_addr(usize addr) -> int {
  assume(addr > CACHE_MINBLK_SIZE * 2);
  return cpp::fhsb(addr / (CACHE_MINBLK_SIZE * 2));
}

dlexport auto alloc(usize size) -> void * {
  if (size > CACHE_MAXBLK_SIZE) return ::malloc(size);
  if (cache_addr == null) abort();
  if (size == 0) return cache_addr;
  val id = blk_id_by_size(size);
  if (atom_tas(&caches_alloced[id][0])) {
    return (void *)((usize)cache_addr + (CACHE_MINBLK_SIZE << (id + 1)) * 2);
  }
  if (atom_tas(&caches_alloced[id][1])) {
    return (void *)((usize)cache_addr + (CACHE_MINBLK_SIZE << (id + 1)) * 3);
  }
  return ::malloc(size);
}

dlexport void free(void *ptr) {
  if (ptr == null) return;
  if (ptr < cache_addr || ptr >= (const byte *)cache_addr + CACHE_TOTAL_SIZE) ::free(ptr);
  if (ptr == cache_addr) return;
  val addr = (usize)ptr - (usize)cache_addr;
  val id   = blk_id_by_addr(addr);
  atom_clr(&caches_alloced[id][addr & (addr - 1) ? 1 : 0]);
}

} // namespace pl2d::cache
