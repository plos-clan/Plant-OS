#pragma once
#include <define.h>
#include <type.h>

#define FREE_MAX_NUM              4096
#define ERRNO_NOPE                0
#define ERRNO_NO_ENOGHT_MEMORY    1
#define ERRNO_NO_MORE_FREE_MEMBER 2
#define MEM_MAX(a, b)             ((a) > (b) ? (a) : (b))

typedef struct {
  u32 start;
  u32 end; // end和start都等于0说明这个free结构没有使用
} free_member;

typedef struct freeinfo freeinfo;
typedef struct freeinfo {
  free_member *f;
  freeinfo    *next;
} freeinfo;

typedef struct {
  freeinfo *freeinf;
  int       memerrno;
} memory;

extern u32 memsize;

int     mem_free_finf(memory *mem, freeinfo *finf, void *p, u32 size);
void    mem_delete(int pos, freeinfo *finf);
memory *memory_init(u32 start, u32 size);
u32     memtest(u32 start, u32 end);
