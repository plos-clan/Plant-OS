#include <kernel.h>
#include <cpu.h>

#define EFLAGS_AC_BIT     0x00040000
#define CR0_CACHE_DISABLE 0x60000000

typedef u32 uintptr_t;
u32         memtest_sub(u32, u32);
u32         memtest(u32 start, u32 end) {
  char flg486 = 0;
  u32  eflg, cr0, i;

  /* 确认CPU是386还是486以上的 */
  eflg  = asm_get_flags();
  eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
  asm_set_flags(eflg);
  eflg = asm_get_flags();
  if ((eflg & EFLAGS_AC_BIT) != 0) {
    /* 如果是386，即使设定AC=1，AC的值还会自动回到0 */
    flg486 = 1;
  }

  eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
  asm_set_flags(eflg);

  if (flg486 != 0) {
    cr0  = load_cr0();
    cr0 |= CR0_CACHE_DISABLE; /* 禁止缓存 */
    store_cr0(cr0);
  }

  i = memtest_sub(start, end);

  if (flg486 != 0) {
    cr0  = load_cr0();
    cr0 &= ~CR0_CACHE_DISABLE; /* 允许缓存 */
    store_cr0(cr0);
  }
  return i;
}

void swap(free_member *a, free_member *b) {
  free_member temp = *a;
  *a               = *b;
  *b               = temp;
}
int cmp(free_member a, free_member b) {
  return a.end <= b.end;
}
int partition(free_member *arr, int low, int high) {
  free_member pivot = arr[high];
  int         i     = (low - 1);

  for (int j = low; j <= high - 1; j++) {
    if (cmp(arr[j], pivot)) {
      i++;
      swap(&arr[i], &arr[j]);
    }
  }

  swap(&arr[i + 1], &arr[high]);
  return (i + 1);
}

void quicksort(free_member *arr, int low, int high) {
  if (low < high) {
    int pi = partition(arr, low, high);
    quicksort(arr, low, pi - 1);
    quicksort(arr, pi + 1, high);
  }
}
freeinfo *make_next_freeinfo(memory *mem) {
  const int size = FREE_MAX_NUM * sizeof(free_member) + sizeof(freeinfo);
  freeinfo *fi   = NULL;
  freeinfo *finf = mem->freeinf;
  freeinfo *old  = NULL;
  u32       s, n;
  while (finf) {
    old = finf;
    for (int i = 0; i < FREE_MAX_NUM; i++) {
      if (finf->f[i].start + finf->f[i].end == 0) { break; }
      if (finf->f[i].end - finf->f[i].start >= size) {
        u32 start = finf->f[i].start;
        s         = finf->f[i].start;
        n         = finf->f[i].end;
        mem_delete(i, finf);
        fi = (freeinfo *)start;
        break;
      }
    }
    if (fi) { break; }
    finf = finf->next;
  }
  if (!fi) {
    mem->memerrno = ERRNO_NO_ENOGHT_MEMORY;
    return NULL;
  }
  fi->next = 0;
  while (finf) {
    old  = finf;
    finf = finf->next;
  }
  old->next = fi;
  fi->f     = (free_member *)((u32)fi + sizeof(freeinfo));
  for (int i = 0; i < FREE_MAX_NUM; i++) {
    fi->f[i].start = 0;
    fi->f[i].end   = 0;
  }

  if (n - s > size) {
    mem_free_finf(mem, fi, (void *)(s + size), n - s - size); // 一点也不浪费
  }

  return fi;
}
free_member *mem_insert(int pos, freeinfo *finf) {
  int j = 0;
  for (int i = 0; i < FREE_MAX_NUM; i++) {
    if (finf->f[i].start + finf->f[i].end != 0) { ++j; }
  }
  if (j == FREE_MAX_NUM) { return NULL; }
  for (int i = j - 1; i >= pos; i--) {
    u32 debug1 = (u32)(&(finf->f[i + 1]));
    u32 debug2 = (u32)(&(finf->f[i]));
    if (!debug1 || !debug2) {
      //   printk("error!\n");
      while (true)
        ;
    }
    finf->f[i + 1] = finf->f[i];
  }
  return &(finf->f[pos]);
}
free_member *mem_add(freeinfo *finf) {
  int j = -1;
  for (int i = 0; i < FREE_MAX_NUM; i++) {
    if (finf->f[i].start + finf->f[i].end == 0) {
      j = i;
      break;
    }
  }
  if (j == -1) { return NULL; }
  return &(finf->f[j]);
}
void mem_delete(int pos, freeinfo *finf) {
  int i;
  for (i = pos; i < FREE_MAX_NUM - 1; i++) {
    if (finf->f[i].start == 0 && finf->f[i].end == 0) { return; }
    finf->f[i] = finf->f[i + 1];
  }
  finf->f[i].start = 0;
  finf->f[i].end   = 0;
}
u32 mem_get_all_finf(freeinfo *finf) {
  for (int i = 0; i < FREE_MAX_NUM; i++) {
    if (finf->f[i].start + finf->f[i].end == 0) { return i; }
  }
  return FREE_MAX_NUM;
}
// 内存整理
void mem_defragmenter(freeinfo *finf) {
  for (int i = 0; i < FREE_MAX_NUM - 1; i++) {
    if (finf->f[i].start + finf->f[i].end == 0) { break; }
    if (finf->f[i].end - finf->f[i].start == 0) {
      mem_delete(i, finf);
      continue;
    }
    if (finf->f[i].end == finf->f[i + 1].start) {
      int end = finf->f[i + 1].end;
      mem_delete(i + 1, finf);
      finf->f[i].end = end;
      continue;
    }
    if (finf->f[i + 1].start == finf->f[i].start) {
      int end = MEM_MAX(finf->f[i].end, finf->f[i + 1].end);
      mem_delete(i + 1, finf);
      finf->f[i].end = end;
      continue;
    }
    if (finf->f[i + 1].start < finf->f[i].end) {
      int end = MEM_MAX(finf->f[i].end, finf->f[i + 1].end);
      mem_delete(i + 1, finf);
      finf->f[i].end = end;
      continue;
    }
  }
}
int mem_free_finf(memory *mem, freeinfo *finf, void *p, u32 size) {
  quicksort(finf->f, 0, mem_get_all_finf(finf) - 1);
  mem_defragmenter(finf);
  free_member *tmp1 = NULL, // 第一（二）个连续的内存 其limit与start相等
      *tmp2         = NULL; // 第二（一）个连续的内存  其start与limit相等
  int idx1, idx2;
  // 遍历内存池，找到符合条件的两个格子（找不到也没关系）

  for (int i = 0; i < FREE_MAX_NUM; i++) {
    uintptr_t current_start = (uintptr_t)finf->f[i].start;
    uintptr_t current_end   = (uintptr_t)finf->f[i].end;
    uintptr_t ptr_val       = (uintptr_t)p;

    if (current_end == ptr_val) {
      tmp1 = &(finf->f[i]);
      idx1 = i;
    }
    if (current_start == ptr_val + size) {
      tmp2 = &(finf->f[i]);
      idx2 = i;
    }
  }

  if (!tmp1 && !tmp2) {             // 没有内存和他连续
                                    // for(;;);
    free_member *n = mem_add(finf); // 找一个空闲的格子放这块内存
    if (!n) return 0;
    // 配置这个格子
    n->start = (u32)p;
    n->end   = (u32)p + size;
    quicksort(finf->f, 0, mem_get_all_finf(finf) - 1);
    mem_defragmenter(finf);
    return 1;
  }
  // for(;;);
  //  两个都找到了，说明是个缺口
  if (tmp1 && tmp2) {
    tmp1->end = tmp2->end;
    mem_delete(idx2, finf);
    quicksort(finf->f, 0, mem_get_all_finf(finf) - 1);
    mem_defragmenter(finf);
    return 1;
  }
  if (tmp1) { // BUGFIX
    tmp1->end += size;
    quicksort(finf->f, 0, mem_get_all_finf(finf) - 1);
    mem_defragmenter(finf);
    return 1;
  }
  if (tmp2) {
    tmp2->start = (u32)p;
    quicksort(finf->f, 0, mem_get_all_finf(finf) - 1);
    mem_defragmenter(finf);
    return 1;
  }

  return 1;
}
void *mem_alloc_finf(memory *mem, freeinfo *finf, u32 size, freeinfo *if_nomore) {
  free_member *choice       = NULL;
  int          choice_index = 0;
  for (int i = 0; i < FREE_MAX_NUM; i++) {
    if (finf->f[i].start == 0 && finf->f[i].end == 0) { break; }
    if (finf->f[i].end - finf->f[i].start >= size) {
      if (!choice) {
        choice       = &(finf->f[i]);
        choice_index = i;
        continue;
      }
      if (finf->f[i].end - finf->f[i].start < choice->start - choice->end) {
        choice       = &(finf->f[i]);
        choice_index = i;
        continue;
      }
    }
  }
  if (choice == NULL) {
    mem->memerrno = ERRNO_NO_ENOGHT_MEMORY;
    return NULL;
  }
  u32 start      = choice->start;
  choice->start += size;
  if (choice->end - choice->start == 0) { mem_delete(choice_index, finf); }
  mem->memerrno = ERRNO_NOPE;
  mem_defragmenter(finf);
  memset((void *)start, 0, size);

  return (void *)start;
}
void *mem_alloc(memory *mem, u32 size) {
  freeinfo *finf      = mem->freeinf;
  int       flag      = 0;
  freeinfo *if_nomore = NULL;
  while (finf) {
    if (flag && !if_nomore) {
      break;
      ;
    }
    void *result = mem_alloc_finf(mem, finf, size, if_nomore);
    if (mem->memerrno != ERRNO_NOPE) {
      if (mem->memerrno == ERRNO_NO_MORE_FREE_MEMBER) {
        if (!flag) {
          if_nomore = finf;
          flag      = 1;
        }
      }
    } else {
      return result;
    }
    if (flag) {
      if_nomore = if_nomore->next;
    } else {
      finf = finf->next;
    }
  }
  if (flag) {
    freeinfo *new_f = make_next_freeinfo(mem);
    if (!new_f) { return NULL; }
    return mem_alloc(mem, size);
  }
  return NULL;
}
void mem_free(memory *mem, void *p, u32 size) {
  freeinfo *finf = mem->freeinf;
  while (finf) {
    if (mem_free_finf(mem, finf, p, size)) { return; }
    finf = finf->next;
  }
  freeinfo *new_f = make_next_freeinfo(mem);
  if (new_f) { mem_free_finf(mem, new_f, p, size); }
}
void show_mem(memory *mem) {
  //   printk("----------------\n");
  //   freeinfo *finf = mem->freeinf;
  //   while (finf) {
  //     for (int i = 0; i < FREE_MAX_NUM; i++) {
  //       if (finf->f[i].start == 0 && finf->f[i].end == 0) { break; }
  //       printk("START: %08x END: %08x SIZE: %08x Bytes\n", finf->f[i].start, finf->f[i].end,
  //              finf->f[i].end - finf->f[i].start);
  //     }
  //     finf = finf->next;
  //   }
  //   printk("----------------\n");
}
memory *memory_init(u32 start, u32 size) {
  memory *mem;
  mem    = (memory *)start;
  start += sizeof(memory);
  size  -= sizeof(memory);
  if (size < 0) {
    //  printk("mm init error.\n");
    while (true)
      ;
  }
  mem->freeinf  = (freeinfo *)start;
  start        += sizeof(freeinfo);
  size         -= sizeof(freeinfo);
  if (size < 0) {
    //  printk("mm init error.\n");
    while (true)
      ;
  }
  mem->freeinf->next  = 0;
  mem->freeinf->f     = (free_member *)start;
  start              += FREE_MAX_NUM * sizeof(free_member);
  size               -= FREE_MAX_NUM * sizeof(free_member);
  if ((int)size < 0) {
    //   printk("mm init error.\n");
    while (true)
      ;
  }
  for (int i = 0; i < FREE_MAX_NUM; i++) {
    mem->freeinf->f[i].start = 0;
    mem->freeinf->f[i].end   = 0;
  }
  mem->memerrno = ERRNO_NOPE;
  mem_free(mem, (void *)start, size);
  return mem;
}
extern memory *public_heap;
void          *malloc(size_t size) {
  void *p;
  p = mem_alloc(public_heap, size + sizeof(int));
  if (p == NULL) return NULL;
  *(int *)p = size;
  return (char *)p + sizeof(int);
}
void free(void *p) {
  if (p == NULL) return;
  int size = *(int *)(p - sizeof(int));
  mem_free(public_heap, (char *)p - sizeof(int), size + sizeof(int));
}
void *realloc(void *ptr, u32 size) {
  void *new = malloc(size);
  if (ptr) {
    memcpy(new, ptr, *(int *)((int)ptr - 4));
    free(ptr);
  }
  return new;
}

void *kmalloc(int size) {
  void *p;
  p = page_malloc(size + sizeof(int));
  if (p == NULL) return NULL;
  *(int *)p = size;
  return (char *)p + sizeof(int);
}
void kfree(void *p) {
  if (p == NULL) return;
  int size = *(int *)(p - sizeof(int));
  page_free((char *)p - sizeof(int), size + sizeof(int));
}
void *krealloc(void *ptr, u32 size) {
  void *new = kmalloc(size);
  if (ptr) {
    memcpy(new, ptr, *(int *)((int)ptr - 4));
    kfree(ptr);
  }
  return new;
}