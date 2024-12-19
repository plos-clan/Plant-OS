// This code is released under the MIT License

#include <kernel.h>

typedef struct resman {
  avltree_t avltree;
  i32       next_id;
} *resman_t;

typedef struct file_describtor {
  vfs_node_t node;
  i32        refrence_cout;
} *file_describtor_t;

list_t file_descrabtors;
void resman_init(resman_t man);

static i32 alloc_next_id(resman_t man) {
  while (avltree_get(man->avltree, man->next_id++))
    ;
  return man->next_id - 1;
}
resman_t resman_alloc() {
  resman_t man = malloc(sizeof(struct resman));
  resman_init(man);
  return man;
}

void resman_init(resman_t man) {
  if (man == null) return;
  man->avltree = null;
  man->next_id = 0;
}

int  resman_open(i32 working_dir, cstr path) {}
void resman_deinit(resman_t man) {
  avltree_free(man->avltree);
}
