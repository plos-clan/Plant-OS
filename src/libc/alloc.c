// #include <libc.h>

// // 0: 已分配位
// // 1: 块开始/结束位
// // 2: mmap位
// // 3
// // 4-63: 大小

// // 4096 / 4
// // 1024
// //
// // 128

// // #define __alloc_stats

// #ifdef __alloc_stats
// #  undef __alloc_stats
// #  define __alloc_stats(expr) expr
// #else
// #  define __alloc_stats(expr)
// #endif

// #if __alloc_stats(1) + 0
// static struct alloc_stats {
//   struct {
//     atomic_size_t alloc_print_stat;
//     atomic_size_t         //
//         _brk,             //
//         _mmap,            //
//         _mremap,          //
//         _munmap;          //
//     atomic_size_t         //
//         blk_do_split,     //
//         blk_split;        //
//     atomic_size_t         //
//         alist_put,        //
//         alist_do_rm,      //
//         alist_rm,         //
//         alist_matchsize,  //
//         alist_matchbegin, //
//         alist_matchend,   //
//         alist_matchptr;   //
//     atomic_size_t         //
//         flist_put,        //
//         flist_do_rm,      //
//         flist_rm,         //
//         flist_matchsize;  //
//     atomic_size_t         //
//         freelists_id,     //
//         freelists_put,    //
//         freelists_rm,     //
//         freelists_match;  //
//     atomic_size_t         //
//         brk_init,         //
//         brk_req,          //
//         brk_rls,          //
//         brk_do_alloc,     //
//         brk_do_free,      //
//         brk_do_realloc,   //
//         brk_alloc,        //
//         brk_free,         //
//         brk_realloc;      //
//   } func;
// } _stats = {};

// int  fprintf(void *, const char *, ...);
// void alloc_print_stat(void *fp) {
//   _call_cnt_inc(alloc_print_stat);

//   struct alloc_stats stats = _stats;

//   fprintf(fp, "函数调用次数统计:\n");
// #  define _printstat(_name) fprintf(fp, "  %20s: %lu\n", #_name, stats.func._name)

//   _printstat(alloc_print_stat);

//   _printstat(_brk);
//   _printstat(_mmap);
//   _printstat(_mremap);
//   _printstat(_munmap);

//   _printstat(blk_do_split);
//   _printstat(blk_split);

//   _printstat(alist_put);
//   _printstat(alist_do_rm);
//   _printstat(alist_rm);
//   _printstat(alist_matchsize);
//   _printstat(alist_matchbegin);
//   _printstat(alist_matchend);
//   _printstat(alist_matchptr);

//   _printstat(flist_put);
//   _printstat(flist_do_rm);
//   _printstat(flist_rm);
//   _printstat(flist_matchsize);

//   _printstat(freelists_id);
//   _printstat(freelists_put);
//   _printstat(freelists_rm);
//   _printstat(freelists_match);

//   _printstat(brk_init);
//   _printstat(brk_req);
//   _printstat(brk_rls);
//   _printstat(brk_do_alloc);
//   _printstat(brk_do_free);
//   _printstat(brk_do_realloc);
//   _printstat(brk_alloc);
//   _printstat(brk_free);
//   _printstat(brk_realloc);

// #  undef _printstat
// }

// // 将一个函数的被调用计数+1
// #  define _call_cnt_inc(_name) _stats.func._name++
// #else
// #  define _call_cnt_inc(_name)
// #endif

// //* ----------------------------------------------------------------------------------------------------

// #define _size_word  (sizeof(size_t))
// #define _size_2word (2 * sizeof(size_t))

// // 按照指定大小对齐后的大小
// #define _align_mask(s, m) (((size_t)(s) + (m)) & ~(size_t)(m))
// #define _align_n(s, n)    _align_mask(s, (n)-1)
// #define _align_8(s)       _align_n(s, 8)
// #define _align_16(s)      _align_n(s, 16)
// #define _align_64(s)      _align_n(s, 64)
// #define _align_128(s)     _align_n(s, 128)
// #define _align_256(s)     _align_n(s, 256)
// #define _align_512(s)     _align_n(s, 512)
// #define _align_1k(s)      _align_n(s, 1024)
// #define _align_4k(s)      _align_n(s, 4096)
// #define _align_8k(s)      _align_n(s, 8192)
// #define _align_16k(s)     _align_n(s, 16384)
// // 按照字长对齐
// #define _align_word(s)    _align_n(s, _size_word)
// // 按照2倍字长对齐
// #define _align_2word(s)   _align_n(s, _size_2word)
// // 内存分配的对齐
// #define _alloc_align(s)   ((s) ? (_align_2word(s)) : (_size_2word))
// // 检查分配的内存是否已对齐
// #define _align_chk(ptr)                                                                            \
//   ((void)({                                                                                        \
//     if ((size_t)(ptr) & (_size_2word - 1)) abort();                                                \
//   }))
// #define _align_4k_chk(ptr)                                                                         \
//   ((void)({                                                                                        \
//     if ((size_t)(ptr)&4095) abort();                                                               \
//   }))

// #define _trunc_mask(s, m) ((size_t)(s) & ~(size_t)(m))
// #define _trunc_n(s, n)    _trunc_mask(s, (n)-1)
// #define _trunc_8(s)       _trunc_n(s, 8)

// //* ----------------------------------------------------------------------------------------------------
// //* 内存块

// // 内存块头部
// typedef struct blk_head {
//   size_t size;
//   byte   data[];
// } *blk_head_t;
// // 内存块尾部
// typedef struct blk_foot {
//   size_t size;
// } *blk_foot_t;

// #define blk_head_size (sizeof(struct blk_head))
// #define blk_foot_size (sizeof(struct blk_foot))
// #define blk_tag_size  (blk_head_size + blk_foot_size)

// // p: blk_head_t或blk_foot_t
// #define blk_alloced(p) ((p)->size & 1)

// // p: 当前块指针
// #define blk_size_b(p)        (*(size_t *)(p))
// // p: 当前块指针, s: 当前块大小
// #define blk_size_e(p, s)     (*((size_t *)((void *)(p) + (s)) + 1))
// // p: 当前块指针, s: 上一块大小
// #define blk_lastsize_b(p, s) (*((size_t *)((void *)(p) - (s)) - 2))
// // p: 当前块指针
// #define blk_lastsize_e(p)    (*((size_t *)(p)-1))
// // p: 当前块指针, s: 上一块大小
// #define blk_last_(p, s)      ((void *)(p) - (s) - sizeof(size_t) * 2)
// // p: 当前块指针, s: 当前块大小
// #define blk_next_(p, s)      ((void *)(p) + (s) + sizeof(size_t) * 2)
// // p: 当前块指针, s: 当前块大小
// #define blk_nextsize_(p, s)  blk_size_b(blk_next_(p, s))

// // p: 当前块指针
// #define blk_size(p)     blk_size_b(p)
// // p: 当前块指针
// #define blk_lastsize(p) blk_lastsize_e(p)
// // p: 当前块指针
// #define blk_last(p)     blk_last_(p, blk_lastsize(p))
// // p: 当前块指针
// #define blk_next(p)     blk_next_(p, blk_size(p))
// // p: 当前块指针
// #define blk_nextsize(p) blk_size(blk_next(p))

// // p: 当前块指针, s: 当前块大小
// #define blk_setsize(p, s)                                                                          \
//   do {                                                                                             \
//     size_t __size         = (s);                                                                   \
//     blk_size_b(p)         = __size;                                                                \
//     blk_size_e(p, __size) = __size;                                                                \
//   } while (0)
// // p: 当前块指针
// #define blk_setalloced(p)                                                                          \
//   do {                                                                                             \
//     size_t __size           = blk_size(p);                                                         \
//     size_t __size_with_flag = (__size | 1);                                                        \
//     blk_size_b(p)           = __size_with_flag;                                                    \
//     blk_size_e(p, __size)   = __size_with_flag;                                                    \
//   } while (0)
// // p: 当前块指针 s: 当前块大小
// #define blk_setfreeed_(p, s)                                                                       \
//   do {                                                                                             \
//     blk_size_b(p)    = (s);                                                                        \
//     blk_size_e(p, s) = (s);                                                                        \
//   } while (0)
// // p: 当前块指针
// #define blk_setfreeed(p)                                                                           \
//   do {                                                                                             \
//     size_t __size         = _trunc_8(blk_size(p));                                                 \
//     blk_size_b(p)         = __size;                                                                \
//     blk_size_e(p, __size) = __size;                                                                \
//   } while (0)

// //; 拆分内存块
// //; size原内存块的新大小
// //; split拆分出的新内存块的大小
// //; size与split必须对齐
// _il_ blk_head_t blk_do_split(blk_head_t p, size_t size, size_t split) {
//   _call_cnt_inc(blk_do_split);
//   blk_setsize(p, size);
//   blk_head_t next = blk_next_(p, size);
//   blk_setsize(next, split);
//   return next;
// }
// //; 拆分内存块 (拆分出的新内存块会尽量大)
// //; size原内存块的最小大小
// //; split拆分出的新内存块的最小大小
// //; size与split必须对齐
// _il_ blk_head_t blk_split(blk_head_t p, size_t size, size_t split) {
//   _call_cnt_inc(blk_split);
//   size_t  blksize  = _trunc_8(p->size);
//   ssize_t sizediff = (ssize_t)blksize - (ssize_t)size - (ssize_t)blk_tag_size;
//   if (sizediff < (ssize_t)split) return NULL;
//   return blk_do_split(p, size, sizediff); // 拆分块
// }

// //; 将p指向的块与它的下一个块合并
// //; p不能为NULL
// // _il_ size_t blk_mergenext(void *p) {
// //   size_t size     = blk_size(p);
// //   void  *next     = blk_next_(p, size);
// //   size_t nextsize = blk_size(next);
// //   brk_freelist_rm(next, freelists_id(nextsize));
// //   size += nextsize + 2 * sizeof(size_t);
// //   blk_setsize(p, size);
// // }
// // _il_ size_t blk_mergelast(void *p) {
// //   size_t size     = blk_size(p);
// //   void  *next     = blk_next_(p, size);
// //   size_t nextsize = blk_size(next);
// //   brk_freelist_rm(next, freelists_id(nextsize));
// //   size += nextsize + 2 * sizeof(size_t);
// //   blk_setsize(p, size);
// // }

// //* ----------------------------------------------------------------------------------------------------
// //* 空闲链表
// //* (线程不安全)

// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// //~ alloclist
// typedef struct alist {
//   size_t        size; // 块大小
//   struct alist *next; // 下一项
//   void         *ptr;  // 块开始处指针
//   void         *end;  // 块结束处指针
// } *alist_t;

// //; 将内存块放入alist
// //; list_p与p不能为NULL
// _il_ void alist_put(alist_t *list_p, alist_t p) {
//   _call_cnt_inc(alist_put);
//   p->next = *list_p;
//   *list_p = p;
// }
// //; 将p指向的内存块从由list_p开始的alist中移除
// //; last: p的上一项，若没有则为NULL
// //; list_p与p不能为NULL
// _il_ void alist_do_rm(alist_t *list_p, alist_t last, alist_t p) {
//   _call_cnt_inc(alist_do_rm);
//   if (last)
//     last->next = p->next;
//   else
//     *list_p = p->next;
// }
// //; 将内存块从alist中移除
// //; 找到内存块返回0, 未找到返回-1
// _il_ int alist_rm(alist_t *list_p, alist_t p) {
//   _call_cnt_inc(alist_rm);
//   alist_t last = NULL, node = *list_p;
//   for (; node; last = node, node = node->next)
//     if (node == p) {
//       alist_do_rm(list_p, last, node);
//       return 0;
//     }
//   return -1;
// }
// //; 在alist中匹配指定大小的块
// //; size与maxsize必须对齐
// _il_ alist_t alist_matchsize(alist_t *list_p, size_t size, size_t maxsize, bool remove) {
//   _call_cnt_inc(alist_matchsize);
//   if (maxsize == 0) maxsize = __SIZE_MAX__;
//   alist_t last = NULL, node = *list_p;
//   for (; node; last = node, node = node->next) {
//     if (node->size >= size && node->size <= maxsize) { // 如果匹配到合适的块
//       alist_do_rm(list_p, last, node);                 // 把块从alist里移除
//       return node;
//     }
//   }
//   return NULL; // 匹配失败
// }
// //; 匹配内存的开始处
// _il_ alist_t alist_matchbegin(alist_t *list_p, void *begin, bool remove) {
//   _call_cnt_inc(alist_matchbegin);
//   alist_t last = NULL, node = *list_p;
//   for (; node; last = node, node = node->next)
//     if (node->ptr == begin) { // 如果匹配到合适的块
//       if (remove)             // 把块从alist里移除
//         alist_do_rm(list_p, last, node);
//       return node;
//     }
//   return NULL; // 匹配失败
// }
// _il_ alist_t alist_matchend(alist_t *list_p, void *end, bool remove) {
//   _call_cnt_inc(alist_matchend);
//   alist_t last = NULL, node = *list_p;
//   for (; node; last = node, node = node->next)
//     if (node->end == end) { // 如果匹配到合适的块
//       if (remove)           // 把块从alist里移除
//         alist_do_rm(list_p, last, node);
//       return node;
//     }
//   return NULL; // 匹配失败
// }
// _il_ alist_t alist_matchptr(alist_t *list_p, void *ptr, bool remove) {
//   _call_cnt_inc(alist_matchptr);
//   alist_t last = NULL, node = *list_p;
//   for (; node; last = node, node = node->next)
//     if (node->ptr <= ptr && ptr < node->end) { // 如果匹配到合适的块
//       if (remove)                              // 把块从alist里移除
//         alist_do_rm(list_p, last, node);
//       return node;
//     }
//   return NULL; // 匹配失败
// }

// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// //~ freelist
// typedef struct flist {
//   size_t        size; // 块大小
//   struct flist *next; // 下一项
//   struct flist *prev; // 上一项
// } *flist_t;

// //; 将内存块放入flist
// //; list_p与p不能为NULL
// _il_ void flist_put(flist_t *list_p, flist_t p) {
//   _call_cnt_inc(flist_put);
//   if (*list_p) (*list_p)->prev = p;
//   p->next = *list_p;
//   *list_p = p;
//   p->prev = NULL;
// }
// //; 将p指向的内存块从由list_p开始的flist中移除
// //; last: p的上一项，若没有则为NULL
// //; list_p与p不能为NULL
// _il_ void flist_do_rm(flist_t *list_p, flist_t p) {
//   _call_cnt_inc(flist_do_rm);
//   flist_t next = p->next, prev = p->prev;
//   if (prev)
//     prev->next = next;
//   else
//     *list_p = next;
//   if (next) next->prev = prev;
// }
// //; 将内存块从flist中移除
// //; 找到内存块返回0, 未找到返回-1
// _il_ int flist_rm(flist_t *list_p, flist_t p) {
//   _call_cnt_inc(flist_rm);
//   for (flist_t node = *list_p; node; node = node->next)
//     if (node == p) {
//       flist_do_rm(list_p, node);
//       return 0;
//     }
//   return -1;
// }
// //; 在flist中匹配指定大小的块
// //; size与maxsize必须对齐
// _il_ blk_head_t flist_matchsize(flist_t *list_p, size_t size, size_t maxsize, bool remove) {
//   _call_cnt_inc(flist_matchsize);
//   if (maxsize == 0) maxsize = __SIZE_MAX__;
//   for (flist_t node = *list_p; node; node = node->next)
//     if (node->size >= size && node->size <= maxsize) { // 如果匹配到合适的块
//       flist_do_rm(list_p, node);                       // 把块从flist里移除
//       return (blk_head_t)node;
//     }
//   return NULL; // 匹配失败
// }

// //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// #define afrray_len 16
// typedef struct afarray {
//   struct {
//     size_t size; // 块大小
//     void  *ptr;  // 块开始处指针
//     void  *end;  // 块结束处指针
//   } data[afrray_len];
//   struct afarray *next; // 下一项
// } *afarray_t;

// //* ----------------------------------------------------------------------------------------------------

// // 29个指定大小了类别 1个无限大小的类别
// #define freelists_len 30

// // freelist的数组
// typedef flist_t freelists_t[freelists_len];

// //; 获取该大小属于freelists中的哪个list
// _il_ int freelists_id(size_t size) {
//   _call_cnt_inc(freelists_id);
//   // 16 (最小组)
//   // 32
//   // 48

//   // 16384 (最大)
//   if (size <= 8) abort();
//   if (size <= 16) return 1; // 16
//   if (size <= 32) return 2; // 32
//   if (size <= 4096) {
//     size_t n = ((1 << (fhsb64(size) - 1)) - 1);
//     size     = (size + n) & ~n;
//     n        = fhsb64(size);
//     return ((n - 4) << 1) | ((size >> (n - 1)) & 1);
//   }
//   if (size <= 16384) {
//     size = _align_1k(size);
//     return ((size - 4096) >> 10) + 16;
//   }
//   return freelists_len - 1;
// }

// //; 将块放入freelists中的指定list
// _il_ void freelists_put(void *p, int id, freelists_t fl) {
//   _call_cnt_inc(freelists_put);
//   flist_put(fl + id, p);
// }
// //; 从freelist中移除块
// //; fl不能为NULL
// _il_ void freelists_rm(void *p, int id, freelists_t fl) {
//   _call_cnt_inc(freelists_rm);
//   if (flist_rm(fl + id, p) < 0) abort();
// }
// //; 在freelists中匹配指定大小的块
// //; size必须对齐
// //; 从freelists中指定id的list开始搜索
// //; reverse指定是从小往大搜索(0)还是从大往小搜索(1)
// _il_ void *freelists_match(size_t size, int id, freelists_t fl, size_t split, bool reverse) {
//   _call_cnt_inc(freelists_match);
//   int _id;
//   if (reverse) {
//     _id = id;
//     id  = freelists_len - 1;
//   }

// begin:
//   // 在freelist中匹配内存块
//   void *node = flist_matchsize(fl + id, size, 0, true);
//   if (node) {
//     if (split) {
//       void *newnode = blk_split(node, size, split); // 尝试拆分内存块
//       if (newnode)                                  // 把拆出来的内存块放进freelist里
//         freelists_put(newnode, freelists_id(blk_size(newnode)), fl);
//     }
//     return node; // 匹配成功
//   }

//   if (reverse) goto reverse;
//   if (++id < freelists_len) goto begin;
//   return NULL; // 匹配失败

// reverse:
//   if (--id >= _id) goto begin;
//   return NULL; // 匹配失败
// }

// //* ----------------------------------------------------------------------------------------------------
// //* 从堆中申请内存
// //* 0 <= 申请大小 <16384

// struct mempool {
//   flist_t     pages;
//   mutex_t     lk;
//   void       *begin;
//   void       *end;
//   size_t      len;
//   freelists_t fl;
// };

// //; 初始化内存池
// //; m不能为NULL
// void mempool_init(struct mempool *m) {
//   mutex_init(&m->lk);
//   m->begin = NULL;
//   m->end   = NULL;
//   m->len   = 0;
//   for (int i = 0; i < freelists_len; i++)
//     m->fl[i] = NULL;
// }

// static mutex_t     brk_lk       = MUTEX_INITIALIZER; // 堆互斥锁
// static void       *brk_begin    = NULL;              // 堆开始处
// static void       *brk_end      = NULL;              // 堆结束处
// static size_t      brk_len      = 0;                 // 堆大小
// static freelists_t brk_freelist = {};                // 堆空闲链表
// static freelists_t brk_quick    = {};

// //; 指针指向的对象是否在堆中
// bool is_in_brk(void *p) {
//   return brk_begin <= p && p < brk_end;
// }

// //; 锁定堆
// _il_ int brk_lock() {
//   return mutex_lock(&brk_lk);
// }
// //; 解锁堆
// _il_ int brk_unlock() {
//   return mutex_unlock(&brk_lk);
// }

// //; 将块放入freelist
// //; 必须先锁定堆
// _il_ void brk_freelist_put(void *p, int id) {
//   freelists_put(p, id, brk_freelist);
// }
// //; 从freelist中移除块
// //; 必须先锁定堆
// _il_ void brk_freelist_rm(void *p, int id) {
//   freelists_rm(p, id, brk_freelist);
// }

// //; 初始化堆
// int brk_init(bool locked) {
//   _call_cnt_inc(brk_init);
//   if (brk_begin) return 0;
//   if (!locked) mutex_lock(&brk_lk);
//   if (!brk_begin) brk_end = brk_begin = _brk(NULL);
//   void *p = brk_begin;
//   if (!locked) mutex_unlock(&brk_lk);
//   if (!p) return -ENOMEM;
//   return 0;
// }
// //; 申请扩大堆
// //; 可用内存至少应扩大size
// int brk_req(size_t size, bool locked) {
//   _call_cnt_inc(brk_req);
//   if (!size) return -EINVAL;        // 请求增加的大小不能为0
//   if (!brk_begin) {                 // 堆未初始化
//     brk_init(locked);               // 初始化
//     if (!brk_begin) return -ENOMEM; // 内存不足
//     // (第一次在堆上分配空间)
//     size += blk_tag_size; // 因为堆前后有两个标记，所以请求的空间要增加
//   }
//   size += blk_tag_size; // 因为块前后有两个大小标记，所以请求的空间要增加

//   size = _align_4k(size); // 4k对齐

//   if (!locked) mutex_lock(&brk_lk); //^ 加锁

//   void *end    = brk_end;        // 旧的堆结束位置
//   void *begin  = brk_begin;      // 堆开始位置
//   void *newbrk = end + size;     // 计算新的堆顶
//   void *newend = _brk(newbrk);   // 扩大堆
//   size         = newend - end;   // 计算扩大的大小
//   brk_end      = newend;         // 更新堆顶的位置
//   brk_len      = newend - begin; // 更新堆大小

//   if (!size) goto end; // 堆未扩大

//   blk_lastsize_e(newend) = ~(size_t)0; // 在堆的(新)结束处写入标记

//   if (end == begin) {                      // 第一次在堆上分配空间
//     blk_size(begin)  = ~(size_t)0;         // 在堆的开始处写入标记
//     size            -= 4 * sizeof(size_t); // 减去(堆和块)标记的大小
//     begin           += sizeof(size_t);
//     blk_setsize(begin, size);                    // 写入块的大小
//     brk_freelist_put(begin, freelists_id(size)); // 将块放入空闲表
//     _align_chk(begin + sizeof(size_t));
//     goto end;
//   }

//   end -= sizeof(size_t); // 将旧的堆结束指针指向旧的堆结束标记
//   ;                      // 之后的操作会覆盖旧的堆结束标记

//   size_t lastsize = blk_lastsize(end);         // 获取堆最后一个块的信息
//   if (lastsize & 1) {                          // 最后一个块已分配
//     size -= 2 * sizeof(size_t);                // 减去标记的大小
//     blk_setsize(end, size);                    // 写入块的大小
//     brk_freelist_put(end, freelists_id(size)); // 将块放入空闲表
//     _align_chk(end + sizeof(size_t));
//     goto end;
//   }

//   blk_head_t last = blk_last_(end, lastsize);     // 获取最后一个块
//   brk_freelist_rm(last, freelists_id(lastsize));  // 将块移出空闲表
//   lastsize += size;                               // 计算块的总大小
//   blk_setsize(last, lastsize);                    // 写入大小
//   brk_freelist_put(last, freelists_id(lastsize)); // 将块放入空闲表

//   _align_chk(last->data);
// end:
//   if (!locked) mutex_unlock(&brk_lk); //^ 解锁
//   if (newend < newbrk) return -ENOMEM;
//   return 0;
// }
// // 申请缩小堆
// //; 还没做好
// int brk_rls(size_t size) {
//   _call_cnt_inc(brk_rls);
//   if (!size) return -EINVAL; // 请求减小的大小不能为0
//   if (!brk_begin) return -EINVAL;
//   size         &= ~(size_t)4095;
//   void *newbrk  = brk_end - size;
//   brk_end       = _brk(newbrk);
//   brk_len       = brk_end - brk_begin;
//   return 0;
// }

// //; 从堆中分配一个块
// //; 注意: size必须按2倍字长对齐且不能为0
// //; 返回的是块的开始地址，请注意：块开始处还有一个字长的区域是不能使用的！
// //; 块的可使用区域按2倍字长对齐，而不是块开始处按2倍字长对齐
// blk_head_t brk_do_alloc(size_t size, bool locked) {
//   _call_cnt_inc(brk_do_alloc);
//   _align_chk(size);            // 检查对齐
//   int id = freelists_id(size); // 获取块所在空闲链表的长度

//   if (!locked) mutex_lock(&brk_lk); //^ 加锁

//   if (brk_quick[id] && brk_quick[id]->size >= size) {
//     blk_head_t ptr = (blk_head_t)brk_quick[id];
//     brk_quick[id]  = NULL;
//     if (!locked) mutex_unlock(&brk_lk); //^ 解锁
//     return ptr;
//   }

//   if (!brk_begin) goto req_mem; //^ 第一次分配
//   blk_head_t ptr = freelists_match(size, id, brk_freelist, _size_2word, false);
//   if (!ptr) {
//   req_mem:
//     if (brk_req(size, true) < 0) return NULL;
//     ptr = freelists_match(size, id, brk_freelist, _size_2word, false);
//   }
//   if (ptr) blk_setalloced(ptr); // 设置块为已分配

//   if (!locked) mutex_unlock(&brk_lk); //^ 解锁

//   _align_chk(ptr->data);
//   return ptr;
// }
// //; 将一个块释放
// //; 注意: ptr必须指向合法内存块
// //; ptr指向的是块的开始地址，而不是块的可使用区域
// void brk_do_free(blk_head_t ptr, bool locked) {
//   _call_cnt_inc(brk_do_free);
//   _align_chk(ptr->data); //$ 对齐检查

//   size_t size = _trunc_8(ptr->size);
//   int    id   = freelists_id(ptr->size); // 获取块所在空闲链表的长度

//   if (!locked) mutex_lock(&brk_lk); //^ 加锁

//   if (!brk_quick[id]) {
//     brk_quick[id] = (flist_t)ptr;
//     if (!locked) mutex_unlock(&brk_lk); //^ 解锁
//     return;
//   } else if (brk_quick[id]->size < size) {
//     blk_head_t old = (blk_head_t)brk_quick[id];
//     brk_quick[id]  = (flist_t)ptr;
//     ptr            = old;
//     size           = _trunc_8(ptr->size);
//   }

//   blk_setfreeed_(ptr, size); // 设为已释放

//   void  *next     = blk_next_(ptr, size);
//   size_t nextsize = blk_size(next);
//   if (~nextsize && (nextsize & 1) == 0) { // 之后还有块 && 之后的块未被分配
//     brk_freelist_rm(next, freelists_id(nextsize));
//     size += nextsize + 2 * sizeof(size_t);
//     blk_setsize(ptr, size);
//     _align_chk(ptr->data); //$ 对齐检查
//   }
//   size_t lastsize = blk_lastsize(ptr);
//   if (~lastsize && (lastsize & 1) == 0) { // 之前还有块 && 之前的块未被分配
//     ptr = blk_last_(ptr, lastsize);
//     brk_freelist_rm(ptr, freelists_id(lastsize));
//     size += lastsize + 2 * sizeof(size_t);
//     blk_setsize(ptr, size);
//     _align_chk(ptr->data); //$ 对齐检查
//   }

//   _align_chk(ptr->data);                     //$ 对齐检查
//   brk_freelist_put(ptr, freelists_id(size)); // 将块放入空闲表

//   if (!locked) mutex_unlock(&brk_lk); //^ 解锁
// }
// //;
// blk_head_t brk_do_realloc(blk_head_t ptr, size_t newsize, bool locked) {
//   _call_cnt_inc(brk_do_realloc);
//   _align_chk(ptr->data); //$ 对齐检查

//   size_t size = _trunc_8(ptr->size);
//   _align_chk(size); //$ 对齐检查

//   if (newsize == size) return ptr;

//   if (newsize < size) {
//     if (!locked) mutex_lock(&brk_lk); //^ 加锁
//     blk_head_t newblk = blk_split(ptr, newsize, _size_2word);
//     if (newblk) {
//       brk_freelist_put(newblk, freelists_id(newblk->size));
//       blk_setalloced(ptr);
//     }
//     if (!locked) mutex_unlock(&brk_lk); //^ 解锁
//     return ptr;
//   } else {
//     blk_head_t newblk = brk_do_alloc(newsize, locked); // 分配新的块
//     if (!newblk) return NULL;
//     memcpy(newblk->data, ptr->data, size);
//     brk_do_free(ptr, locked);
//     return newblk;
//   }
// }

// void *brk_alloc(size_t size) {
//   _call_cnt_inc(brk_alloc);
//   size           = _alloc_align(size); // 对齐内存
//   blk_head_t ptr = brk_do_alloc(size, false);
//   if (!ptr) return NULL;          // 内存不足
//   if (!blk_alloced(ptr)) abort(); // 检查分配到的内存
//   _align_chk(ptr->data);          //$ 对齐检查
//   return ptr->data;
// }
// void brk_free(void *ptr) {
//   _call_cnt_inc(brk_free);
//   if (!ptr) return;
//   _align_chk(ptr); //$ 对齐检查
//   ptr -= _size_word;
//   if (!blk_alloced((blk_head_t)ptr)) abort(); // 判断该位置是否已分配
//   brk_do_free(ptr, false);
// }
// void *brk_realloc(void *ptr, size_t size) {
//   _call_cnt_inc(brk_realloc);
//   if (!ptr) return brk_alloc(size);
//   _align_chk(ptr); //$ 对齐检查
//   ptr -= _size_word;
//   if (!blk_alloced((blk_head_t)ptr)) abort(); // 判断该位置是否已分配
//   size              = _alloc_align(size);     // 对齐内存
//   blk_head_t newptr = brk_do_realloc(ptr, size, false);
//   if (!newptr) return NULL;          // 内存不足
//   if (!blk_alloced(newptr)) abort(); // 检查分配到的内存
//   _align_chk(newptr->data);          //$ 对齐检查
//   return newptr->data;
// }

// //* ----------------------------------------------------------------------------------------------------
// //* 向操作系统申请内存

// alist_t page_alloclist    = NULL;
// mutex_t page_alloclist_lk = MUTEX_INITIALIZER;

// // 从系统中申请内存
// // 获得的内存是4k对齐的
// // size可以不是4k的整数倍(会被自动调整为4k的整数倍)
// // size不能为0
// void *page_alloc(size_t size) {
//   size         = _align_4k(size); // 对齐内存
//   alist_t node = brk_alloc(sizeof(struct flist));
//   if (!node) return NULL;
//   void *ptr = _mmap(size); // 向系统请求一块内存
//   if (!ptr) {
//     brk_free(node);
//     return NULL;
//   }
//   node->ptr  = ptr;
//   node->end  = ptr + size;
//   node->size = size;
//   mutex_lock(&page_alloclist_lk);
//   alist_put(&page_alloclist, node);
//   mutex_unlock(&page_alloclist_lk);
//   return ptr;
// }
// // 释放内存到系统中
// void page_free(void *ptr) {
//   _align_4k_chk(ptr); // 检查内存是否4k对齐
//   size_t size = 0;
//   mutex_lock(&page_alloclist_lk);
//   alist_t node = alist_matchbegin(&page_alloclist, ptr, true);
//   if (node) {
//     size = node->size;
//     brk_free(node);
//   } else {
//     abort();
//   }
//   mutex_unlock(&page_alloclist_lk);
//   if (size) _munmap(ptr, size);
// }

// size_t sys_memsize(void *ptr) {
//   size_t size = 0;
//   mutex_lock(&page_alloclist_lk);
//   alist_t node = alist_matchbegin(&page_alloclist, ptr, false);
//   if (node) size = node->size;
//   mutex_unlock(&page_alloclist_lk);
//   return size;
// }

// //* ----------------------------------------------------------------------------------------------------
// //* 包装函数

// void *my_malloc(size_t size) {
//   size = _alloc_align(size); // 对齐内存
//   if (size >= 16384) return page_alloc(size);
//   if (_align_128(size) == _align_4k(size)) return page_alloc(size);
//   return brk_alloc(size);
// }
// void my_free(void *ptr) {
//   if (!ptr) return;
//   if (!is_in_brk(ptr)) {
//     page_free(ptr);
//     return;
//   }
//   if (!brk_begin) return;
//   _align_chk(ptr);
//   brk_free(ptr);
// }
// size_t my_msize(void *ptr) {
//   if (!ptr) return 0;
//   if (!is_in_brk(ptr)) return sys_memsize(ptr);
//   if (!brk_begin) return 0;
//   ptr -= sizeof(size_t);
//   return blk_size(ptr) & ~(size_t)1;
// }

// #undef _il_

// void *malloc(size_t size) {
//   void *ptr = my_malloc(size);
//   _align_chk(ptr);
//   return ptr;
// }
// void *xmalloc(size_t size) {
//   void *ptr = malloc(size);
//   if (!ptr) abort();
//   return ptr;
// }
// void free(void *ptr) {
//   my_free(ptr);
// }
// void *calloc(size_t n, size_t size) {
//   size      *= n;
//   void *ptr  = malloc(size);
//   if (!ptr) return NULL;
//   memset(ptr, 0, size);
//   return ptr;
// }
// void *realloc(void *ptr, size_t newsize) {
//   if (is_in_brk(ptr) && newsize < 4096) return brk_realloc(ptr, newsize);

//   void *newptr = malloc(newsize);
//   if (!newptr) return NULL;
//   size_t size = my_msize(ptr);
//   if (size > newsize) size = newsize;
//   if (ptr) {
//     memcpy(newptr, ptr, size);
//     free(ptr);
//   }
//   return newptr;
// }
// void *reallocarray(void *ptr, size_t n, size_t size) {
//   return realloc(ptr, n * size);
// }
// void *aligned_alloc(size_t align, size_t size) {
//   return page_alloc(size);
// }
// size_t malloc_usable_size(void *ptr) {
//   return my_msize(ptr);
// }
// void *memalign(size_t align, size_t size) {
//   return page_alloc(size);
// }
// int posix_memalign(void **memptr, size_t alignment, size_t size) {
//   *memptr = page_alloc(size);
//   return 0;
// }
// void *pvalloc(size_t size) {
//   return page_alloc(size);
// }
// void *valloc(size_t size) {
//   return page_alloc(size);
// }

// void aligned_realloc() {
//   abort();
// }

// void _aligned_malloc() {
//   abort();
// }
// void _aligned_realloc() {
//   abort();
// }
// void _aligned_offset_malloc() {
//   abort();
// }
// void _aligned_offset_realloc() {
//   abort();
// }
