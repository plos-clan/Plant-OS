// This code is released under the MIT License

#include <kernel.h>

typedef struct resman {
  rbtree_t rbtree;
  i32      next_id;
} *resman_t;

void resman_init(resman_t man);

resman_t resman_alloc() {
  resman_t man = malloc(sizeof(struct resman));
  resman_init(man);
  return man;
}

void resman_init(resman_t man) {
  if (man == null) return;
  man->rbtree  = null;
  man->next_id = 0;
}

void resman_deinit(resman_t man) {
  rbtree_free(man->rbtree);
}
