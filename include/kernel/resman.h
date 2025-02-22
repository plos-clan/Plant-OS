#pragma once
#include "00-include.h"

typedef struct resman {
  avltree_t avltree;
  i32       next_id;
} *resman_t;

typedef struct descriptor {
  vfs_node_t node;
  i32        refrence_cout;
} *describtor_t;
typedef struct handle {
  describtor_t fd;
  isize        offset;
} *handle_t;

resman_t resman_alloc();
void     resman_init(resman_t man);
void     resman_descriptors_init();
int      resman_open(resman_t man, i32 working_dir, cstr path);
int      resman_close(resman_t man, i32 id);
resman_t resman_clone(resman_t man);
void     resman_deinit(resman_t man);
int      resman_set_offset(resman_t man, i32 id, isize offset);

#define resman_free(man)          free(((void *)(man)))
#define resman_open_root_dir(man) resman_open((man), 0, "/")
