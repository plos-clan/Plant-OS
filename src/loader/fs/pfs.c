// libpfs
#include <dosldr.h>
#include <mstr.h>
#include <pfs.h>

void pfs_read_block(pfs_t *pfs, u32 lba, u32 numbers, void *buff) {
  Disk_Read(lba, numbers, buff, pfs->disk_number);
}
void pfs_write_block(pfs_t *pfs, u32 lba, u32 numbers, void *buff) {
  Disk_Write(lba, numbers, buff, pfs->disk_number);
}
#define now_pfs_t ((pfs_t *)(vfs->cache))
/*
  @brief 格式化磁盘为pfs
 */
void pfs_format(pfs_t p, char *volid) {
  return;
}
/*
  @brief 分配一个pfs block
  @return 返回block编号
 */
u32 pfs_alloc_block(vfs_t *vfs, u32 *err) {
  List *l;
  for (int i = 0, k = 1; l = FindForCount(k, now_pfs_t->bitmap); i++, k++) {
    u32 current_block = l->val;
    u8 *bitmap;
    bitmap = (u8 *)FindForCount(k, now_pfs_t->bitmap_buffer)->val;
    for (int j = 0; j < total_bits_of_one_sec; j++) {
      if (!bit_get(bitmap, j)) {
        used(bitmap, j);
        if (j == total_bits_of_one_sec - 1) {
          AddVal(j + i * total_bits_of_one_sec, now_pfs_t->bitmap);
          set_next(bitmap, j + i * total_bits_of_one_sec);
          now_pfs_t->write_block(
              now_pfs_t, i ? block2sector(current_block, now_pfs_t) : current_block, 1, bitmap);
          u8 *bitmap_new = malloc(512); // 默认啥也没使用
          memset(bitmap_new, 0, 512);
          now_pfs_t->write_block(
              now_pfs_t, block2sector((j + i * (total_bits_of_one_sec)), now_pfs_t), 1, bitmap_new);
          AddVal((u32)bitmap_new, now_pfs_t->bitmap_buffer);
          break; // 这个你不能用[doge]
        } else {
          now_pfs_t->write_block(
              now_pfs_t, i ? block2sector(current_block, now_pfs_t) : current_block, 1, bitmap);
          return j + i * (total_bits_of_one_sec);
        }
      }
    }
  }
  if (err) { *err = 0x114514; }
  return 0;
}
void pfs_free_block(vfs_t *vfs, u32 block) {
  u32   index_of_list  = block / total_bits_of_one_sec + 1;
  u32   index_of_block = block % total_bits_of_one_sec;
  List *l              = FindForCount(index_of_list, now_pfs_t->bitmap);
  if (!l) { return; }
  u8 *bm;
  bm = (u8 *)FindForCount(index_of_list, now_pfs_t->bitmap_buffer)->val;
  unused(bm, index_of_block);
  now_pfs_t->write_block(now_pfs_t, index_of_list - 1 ? block2sector(l->val, now_pfs_t) : l->val, 1,
                         bm);
}
u32 pfs_alloc_block_mark(vfs_t *vfs,
                         u32   *err) { // just mark, and save the bitmap to now_pfs_t->bitmap,
                                     // but it wouldn't write the bitmap to the disk
  List *l;
  for (int i = 0, k = 1; l = FindForCount(k, now_pfs_t->bitmap); i++, k++) {
    u32 current_block = l->val;
    u8 *bitmap;
    bitmap = (u8 *)FindForCount(k, now_pfs_t->bitmap_buffer)->val;
    for (int j = 0; j < total_bits_of_one_sec; j++) {
      if (!bit_get(bitmap, j)) {
        used(bitmap, j);
        if (j == total_bits_of_one_sec - 1) {
          AddVal(j + i * total_bits_of_one_sec, now_pfs_t->bitmap);
          set_next(bitmap, j + i * total_bits_of_one_sec);
          now_pfs_t->write_block(
              now_pfs_t, i ? block2sector(current_block, now_pfs_t) : current_block, 1, bitmap);
          u8 *bitmap_new = malloc(512); // 默认啥也没使用
          memset(bitmap_new, 0, 512);
          now_pfs_t->write_block(
              now_pfs_t, block2sector((j + i * (total_bits_of_one_sec)), now_pfs_t), 1, bitmap_new);
          AddVal((u32)bitmap_new, now_pfs_t->bitmap_buffer);
          break; // 这个你不能用[doge]
        } else {
          u32 cb = i ? current_block : current_block - now_pfs_t->first_sec_of_bitmap;
          if (now_pfs_t->current_bitmap_block != -1ll && now_pfs_t->current_bitmap_block != cb) {
            pfs_flush_bitmap(vfs);
            now_pfs_t->write_block(
                now_pfs_t, i ? block2sector(current_block, now_pfs_t) : current_block, 1, bitmap);
            now_pfs_t->current_bitmap_block =
                i ? current_block : current_block - now_pfs_t->first_sec_of_bitmap;
            now_pfs_t->bitmap_buff = bitmap;
          } else if (now_pfs_t->current_bitmap_block == -1ll) {
            now_pfs_t->current_bitmap_block =
                (int64_t)(i ? current_block : current_block - now_pfs_t->first_sec_of_bitmap);
            now_pfs_t->bitmap_buff = bitmap;
          }
          return j + i * (total_bits_of_one_sec);
        }
      }
    }
  }

  if (err) { *err = 0x114514; }
  return 0;
}
void pfs_flush_bitmap(vfs_t *vfs) {
  now_pfs_t->write_block(now_pfs_t, block2sector(now_pfs_t->current_bitmap_block, now_pfs_t), 1,
                         now_pfs_t->bitmap_buff);
  now_pfs_t->current_bitmap_block = -1ll;
  now_pfs_t->bitmap_buff          = NULL;
}
void pfs_free_block_mark(vfs_t *vfs, u32 block) {
  u32   index_of_list  = block / total_bits_of_one_sec + 1;
  u32   index_of_block = block % total_bits_of_one_sec;
  List *l              = FindForCount(index_of_list, now_pfs_t->bitmap);
  if (!l) { return; }
  u8 *bm;
  bm = (u8 *)FindForCount(index_of_list, now_pfs_t->bitmap_buffer)->val;
  unused(bm, index_of_block);
  u32 cb = index_of_list - 1 ? l->val : l->val - now_pfs_t->first_sec_of_bitmap;
  if (now_pfs_t->current_bitmap_block != -1ll && now_pfs_t->current_bitmap_block != cb) {
    pfs_flush_bitmap(vfs);
    now_pfs_t->write_block(now_pfs_t, index_of_list - 1 ? block2sector(l->val, now_pfs_t) : l->val,
                           1, bm);
    now_pfs_t->current_bitmap_block = cb;
    now_pfs_t->bitmap_buff          = bm;
  } else if (now_pfs_t->current_bitmap_block == -1ll) {
    now_pfs_t->current_bitmap_block = cb;
    now_pfs_t->bitmap_buff          = bm;
  }
}

void init_bitmap(vfs_t *vfs) {
  u8 *sec;
  sec = malloc(512);
  now_pfs_t->read_block(now_pfs_t, now_pfs_t->sec_bitmap_start, 1, sec);
  AddVal(now_pfs_t->sec_bitmap_start, now_pfs_t->bitmap);
  AddVal((u32)sec, now_pfs_t->bitmap_buffer);
  while (get_next(sec)) {
    AddVal(get_next(sec), now_pfs_t->bitmap);
    sec = malloc(512);
    now_pfs_t->read_block(now_pfs_t, block2sector(get_next(sec), now_pfs_t), 1, sec);
    AddVal((u32)sec, now_pfs_t->bitmap_buffer);
  }
}
pfs_inode pfs_get_inode_by_index(vfs_t *vfs, u32 index, u32 dict_block) {
  int            flags = 1;
  int            times = 0;
  pfs_dict_block pdb;
  while (dict_block || flags) {
    memset(&pdb, 0, 512);
    now_pfs_t->read_block(now_pfs_t, block2sector(dict_block, now_pfs_t), 1, &pdb);
    if (times == index / 15) { break; }
    dict_block = pdb.next;
    flags      = 0;
    ++times;
  }
  if (times != index / 15) {
    //  pdb.resd[0] = 0x114514;
    pfs_inode i;
    i.type = 0x04;
    return i;
  }
  return pdb.inodes[index % 15];
}
void pfs_set_inode_by_index(vfs_t *vfs, u32 index, u32 dict_block, pfs_inode *inode) {
  int            flags = 1;
  int            times = 0;
  u32            old   = dict_block;
  pfs_dict_block pdb;
  while (dict_block || flags) {
    now_pfs_t->read_block(now_pfs_t, block2sector(dict_block, now_pfs_t), 1, &pdb);
    old        = dict_block;
    dict_block = pdb.next;
    if (times == index / 15) { break; }

    flags = 0;
    ++times;
    memset(&pdb, 0, 512);
  }
  if (times != index / 15) { return; }
  pdb.inodes[index % 15] = *inode;
  now_pfs_t->write_block(now_pfs_t, block2sector(old, now_pfs_t), 1, &pdb);
}
void pfs_make_inode(vfs_t *vfs, u32 index, char *name, u32 type, u32 dict_block) {
  pfs_inode i;
  i.type = type;
  i.dat  = 0;
  i.next = 0;
  i.time = 0x114514;
  i.size = 0;
  pfs_set_inode_by_index(vfs, index, dict_block, &i);

  if (strlen(name) <= 13) {
    memset(i.name, 0, 14);
    memcpy(i.name, name, strlen(name));
    pfs_set_inode_by_index(vfs, index, dict_block, &i);
  } else {
    u32 rest_of_len_of_name, next;
    memcpy(i.name, name, 13);
    name       += 13;
    i.name[13]  = 0xff;
    next        = pfs_create_inode(vfs, dict_block);
    i.next      = next;
    pfs_set_inode_by_index(vfs, index, dict_block, &i);
    rest_of_len_of_name = strlen(name);
    while (rest_of_len_of_name > 0) {
      pfs_inode_of_long_file_name l;
      l.type = 3;
      if (rest_of_len_of_name > 26) {
        memcpy(l.name, name, 26);
        l.name[26]           = 0xff;
        name                += 26;
        rest_of_len_of_name -= 26;
        pfs_set_inode_by_index(vfs, next, dict_block, (pfs_inode *)&l);
        u32 n  = next;
        next   = pfs_create_inode(vfs, dict_block);
        l.next = next;
        pfs_set_inode_by_index(vfs, n, dict_block, (pfs_inode *)&l);
      } else {
        memcpy(l.name, name, rest_of_len_of_name);
        l.name[rest_of_len_of_name]  = 0x00;
        l.name[26]                   = 0x00;
        name                        += rest_of_len_of_name;
        rest_of_len_of_name         -= rest_of_len_of_name;

        pfs_set_inode_by_index(vfs, next, dict_block, (pfs_inode *)&l);
      }
    }
  }
}
void pfs_inode_block_make(vfs_t *vfs, u32 block, u32 next) {
  pfs_dict_block d;
  for (int i = 0; i < 15; i++) {
    d.inodes[i].type = 0;
    d.inodes[i].next = 0;
    d.inodes[i].dat  = 0;
  }
  d.next = next;
  now_pfs_t->write_block(now_pfs_t, block2sector(block, now_pfs_t), 1, &d);
}
u32 pfs_create_inode(vfs_t *vfs, u32 dict_block) {
  int            flags = 1;
  int            times = 0;
  u32            old;
  pfs_dict_block pdb;
  while (dict_block || flags) {
    memset(&pdb, 0, 512);
    now_pfs_t->read_block(now_pfs_t, block2sector(dict_block, now_pfs_t), 1, &pdb);
    for (int i = 0; i < 15; i++) {
      if (pdb.inodes[i].type == 0) { // 找到没有使用的inode
        return i + times * 15;
      }
    }
    old        = dict_block;
    dict_block = pdb.next;
    flags      = 0;
    ++times;
  }
  pdb.next = pfs_alloc_block(vfs, NULL);
  now_pfs_t->write_block(now_pfs_t, block2sector(old, now_pfs_t), 1, &pdb);
  pfs_inode_block_make(vfs, pdb.next, 0);
  now_pfs_t->read_block(now_pfs_t, block2sector(pdb.next, now_pfs_t), 1, &pdb);
  return times * 15;
}
u32 pfs_get_filesize(vfs_t *vfs, char *filename, u32 dict_block, u32 *err) {
  u32 idx;
  u32 err1;
  pfs_get_file_index_by_path(vfs, filename, dict_block, &err1, &idx, &dict_block);
  if (err1 == 0x114514) {
    if (err) { *err = 0x114514; }
    return 0;
  }
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, dict_block);
  if (i.type != 1) {
    if (err) { *err = 0x114514; }
    return 0;
  }
  return i.size;
}
void pfs_ls(vfs_t *vfs, u32 dict_block) {
  int            flags = 1;
  pfs_dict_block pdb;
  while (dict_block || flags) {
    memset(&pdb, 0, 512);

    now_pfs_t->read_block(now_pfs_t, block2sector(dict_block, now_pfs_t), 1, &pdb);
    for (int i = 0; i < 15; i++) {
      if (pdb.inodes[i].type == 3) {
        //        return;
        continue;
      }
      if (pdb.inodes[i].type != 0) {
        if (pdb.inodes[i].name[13] == 0) {
          printf("%s ", pdb.inodes[i].name);
        } else {
          for (int j = 0; j < 13; j++) {
            printf("%c", pdb.inodes[i].name[j]);
          }
          u32 idx;
          idx = pdb.inodes[i].next;
          while (idx) {
            pfs_inode pi;
            pi                             = pfs_get_inode_by_index(vfs, idx, dict_block);
            pfs_inode_of_long_file_name *f = (pfs_inode_of_long_file_name *)&pi;
            if (f->name[26] == 0x0) {
              printf("%s ", f->name);
              break;
            } else {
              for (int k = 0; k < 26; k++) {
                printf("%c", f->name[k]);
              }
            }
            idx = f->next;
          }
        }
      }
    }
    dict_block = pdb.next;
    flags      = 0;
  }
  printf("\n");
}
u32 pfs_get_idx_of_inode_by_name(vfs_t *vfs, char *name, u32 dict_block, u32 *err) {
  int            flags = 1;
  int            times = 0;
  pfs_dict_block pdb;
  while (dict_block || flags) {
    memset(&pdb, 0, 512);
    now_pfs_t->read_block(now_pfs_t, block2sector(dict_block, now_pfs_t), 1, &pdb);
    for (int i = 0; i < 15; i++) {
      if (pdb.inodes[i].type == 3) {
        //        return;
        continue;
      }
      if (pdb.inodes[i].type != 0) {
        mstr *s = mstr_init();
        if (pdb.inodes[i].name[13] == 0) {
          mstr_add_str(s, pdb.inodes[i].name);
        } else {
          for (int j = 0; j < 13; j++) {
            mstr_add_char(s, pdb.inodes[i].name[j]);
          }
          u32 idx;
          idx = pdb.inodes[i].next;
          while (idx) {
            pfs_inode pi;
            pi                             = pfs_get_inode_by_index(vfs, idx, dict_block);
            pfs_inode_of_long_file_name *f = (pfs_inode_of_long_file_name *)&pi;
            if (f->name[26] == 0x0) {
              mstr_add_str(s, f->name);
              break;
            } else {
              for (int k = 0; k < 26; k++) {
                mstr_add_char(s, f->name[k]);
              }
            }
            idx = f->next;
          }
        }
        // printf("%s ", mstr_get(s));
        if (strcmp(mstr_get(s), name) == 0) {
          mstr_free(s);
          return i + times * 15;
        }
        mstr_free(s);
      }
    }
    dict_block = pdb.next;
    flags      = 0;
    ++times;
  }
  if (err) { *err = 0x114514; }
  return 0;
}
void pfs_create_file(vfs_t *vfs, char *filename, u32 dict_block) {
  pfs_make_inode(vfs, pfs_create_inode(vfs, dict_block), filename, 1, dict_block);
}
void pfs_delete_data_block(vfs_t *vfs, u32 start_block) {
  pfs_data_block p;
  now_pfs_t->read_block(now_pfs_t, block2sector(start_block, now_pfs_t), 1, &p);
  u32 next = p.next;
  while (next) {
    memset(&p, 0, 512);
    now_pfs_t->read_block(now_pfs_t, block2sector(next, now_pfs_t), 1, &p);
    pfs_free_block_mark(vfs, next);
    next = p.next;
  }
  pfs_flush_bitmap(vfs);
}
void pfs_delete_dict_block(vfs_t *vfs, u32 start_block) {
  pfs_dict_block p;
  now_pfs_t->read_block(now_pfs_t, block2sector(start_block, now_pfs_t), 1, &p);
  u32 next = p.next;
  while (next) {
    // s printf("next %d\n",next);
    memset(&p, 0, 512);
    now_pfs_t->read_block(now_pfs_t, block2sector(next, now_pfs_t), 1, &p);
    pfs_free_block(vfs, next);
    next = p.next;
  }
}
void pfs_init_data_block(vfs_t *vfs, u32 dict_block) {
  pfs_data_block d;
  d.next = 0;
  now_pfs_t->write_block(now_pfs_t, block2sector(dict_block, now_pfs_t), 1, &d);
}
void pfs_write_file(vfs_t *vfs, char *filename, u32 size, void *buff, u32 dict_block) {
  u32 err;
  u32 idx;
  pfs_get_file_index_by_path(vfs, filename, dict_block, &err, &idx, &dict_block);
  if (err == 0x114514) { return; }
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, dict_block);
  if (i.type != 1) { return; }
  i.time  = 0x114514;
  i.size  = size;
  u32 dat = i.dat;
  if (!dat) {
    i.dat = pfs_alloc_block(vfs, NULL);
    pfs_init_data_block(vfs, i.dat);
  }
  dat = i.dat;
  while (size > 0) {
    pfs_data_block dat_block;
    now_pfs_t->read_block(now_pfs_t, block2sector(dat, now_pfs_t), 1, &dat_block);
    if (size <= 508) {
      if (dat_block.next) { pfs_delete_data_block(vfs, dat_block.next); }
      dat_block.next = 0;
      memcpy(dat_block.data, buff, size);
      buff += size;
      size -= size;
    } else {
      if (!dat_block.next) {
        dat_block.next = pfs_alloc_block_mark(vfs, NULL);
        pfs_init_data_block(vfs, dat_block.next);
      }
      memcpy(dat_block.data, buff, 508);
      buff += 508;
      size -= 508;
    }
    now_pfs_t->write_block(now_pfs_t, block2sector(dat, now_pfs_t), 1, &dat_block);
    dat = dat_block.next;
  }
  pfs_flush_bitmap(vfs);
  pfs_set_inode_by_index(vfs, idx, dict_block, &i);
}
void pfs_read_file(vfs_t *vfs, char *filename, void *buff, u32 dict_block) {
  u32 err;
  u32 idx;
  pfs_get_file_index_by_path(vfs, filename, dict_block, &err, &idx, &dict_block);
  if (err == 0x114514) { return; }
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, dict_block);
  if (i.type != 1) { return; }
  if (!i.dat) { return; }
  u32 next = i.dat;
  // 乐 某个傻逼想打read的过去式的，结果这个傻子加了个ed
  u32 readed = 0;
  while (next) {
    pfs_data_block p;
    now_pfs_t->read_block(now_pfs_t, block2sector(next, now_pfs_t), 1, &p);
    if (i.size - readed <= 508) {
      memcpy(buff, p.data, i.size - readed);
      buff   += i.size - readed;
      readed += i.size - readed;
    } else {
      memcpy(buff, p.data, 508);
      buff   += 508;
      readed += 508;
    }
    if (readed == i.size) { return; }
    next = p.next;
  }
}
u32 pfs_get_dict_block_by_name(vfs_t *vfs, char *name, u32 dict_block, u32 *err) {
  u32 perr;
  u32 idx = pfs_get_idx_of_inode_by_name(vfs, name, dict_block, &perr);
  if (perr == 0x114514) {
    if (err) { *err = 0x114514; }
    return 0;
  }
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, dict_block);
  if (i.type != 2) {
    if (err) {
      *err = 0x114514; // 啊啊啊啊啊啊啊啊这个文件夹不正常
    }
    return 0;
  }
  return i.dat;
}
void pfs_create_dict(vfs_t *vfs, char *name, u32 dict_block) {
  u32 idx = pfs_create_inode(vfs, dict_block);
  pfs_make_inode(vfs, idx, name, 2, dict_block);
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, dict_block);
  i.size      = 0;
  i.time      = 0x114514;
  i.dat       = pfs_alloc_block(vfs, NULL);
  pfs_inode_block_make(vfs, i.dat, 0);
  pfs_set_inode_by_index(vfs, idx, dict_block, &i);
}
u32 pfs_get_dict_number(vfs_t *vfs, u32 dict_block) {
  int            flags = 1;
  pfs_dict_block pdb;
  u32            result = 0;
  while (dict_block || flags) {
    memset(&pdb, 0, 512);
    now_pfs_t->read_block(now_pfs_t, block2sector(dict_block, now_pfs_t), 1, &pdb);
    for (int i = 0; i < 15; i++) {
      if (pdb.inodes[i].type == 3) { continue; }
      if (pdb.inodes[i].type != 0) { result++; }
    }
    dict_block = pdb.next;
    flags      = 0;
  }
  return result;
}
void pfs_delete_file(vfs_t *vfs, char *filename, u32 dict_block) {
  u32 err, idx;
  idx = pfs_get_idx_of_inode_by_name(vfs, filename, dict_block, &err);
  if (err == 0x114514) {
    printf("delete err.\n");
    return;
  }
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, dict_block);
  if (i.type != 1) {
    printf("it isn't a file!\n");
    return;
  }
  pfs_delete_name_link(vfs, i.next, dict_block);
  if (i.dat) {
    pfs_delete_data_block(vfs, i.dat);
    pfs_free_block(vfs, i.dat);
  }
  i.dat  = 0;
  i.type = 0;
  pfs_set_inode_by_index(vfs, idx, dict_block, &i);
}
void pfs_delete_dict(vfs_t *vfs, char *name, u32 dict_block) {
  u32 err, idx;
  idx = pfs_get_idx_of_inode_by_name(vfs, name, dict_block, &err);
  if (err == 0x114514) {
    printf("delete err.\n");
    return;
  }
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, dict_block);
  if (i.type != 2) {
    printf("it isn't a dict!\n");
    return;
  }
  if (i.dat) {
    if (pfs_get_dict_number(vfs, i.dat) > 0) {
      printf("The dict must be empty!\n");
      return;
    }
    pfs_delete_dict_block(vfs, i.dat);
    pfs_free_block(vfs, i.dat);
  }
  pfs_delete_name_link(vfs, i.next, dict_block);
  i.type = 0;
  i.dat  = 0;
  pfs_set_inode_by_index(vfs, idx, dict_block, &i);
}
// /pfs/hello.txt
u32 _pfs_get_dict_block_by_path(vfs_t *vfs, char *path, char **end, u32 start_block, u32 *err) {
  if (*path == '/') { /* root */
    start_block = 0;
    path++;
  }
  if (*path == '\0') {
    *end = path;
    return 0;
  }
  u32 flag = 0;
  while (1) {
    char *s1;
    s1 = strchr(path, '/');
    if (!s1) {
      u32 e = 0;
      u32 b = pfs_get_dict_block_by_name(vfs, path, start_block, &e);
      if (e == 0x114514) {
        if (end) { *end = path; }
        return start_block;
      } else {
        return b;
      }
    } else {
      char r = *s1;
      *s1    = 0;
      u32 e  = 0;
      u32 b  = pfs_get_dict_block_by_name(vfs, path, start_block, &e);
      if (e == 0x114514) {
        if (err) { *err = 0x114514; }
        *s1 = r;
        return 0;
      }
      start_block = b;
      path        = s1 + 1;
      *s1         = r;
    }
  }
}
u32 pfs_get_dict_block_by_path(vfs_t *vfs, char *path, char **end, u32 start_block, u32 *err) {
  char *p1 = malloc(strlen(path) + 1);
  strcpy(p1, path);
  char *e1 = NULL;
  u32   err1;
  u32   r = _pfs_get_dict_block_by_path(vfs, p1, &e1, start_block, &err1);
  if (err1 != 0x114514) {
    if (e1) {
      if (end) { *end = path + (e1 - p1); }
    }
  } else {
    if (err) { *err = err1; }
  }
  free(p1);
  return r;
}
void pfs_get_file_index_by_path(vfs_t *vfs, char *path, u32 start_block, u32 *err, u32 *idx,
                                u32 *dict_block) {
  char *e;
  u32   err1;
  u32   b = pfs_get_dict_block_by_path(vfs, path, &e, start_block, &err1);
  if (err1 == 0x114514) {
    if (err) { *err = 0x114514; }
    return;
  }
  u32 i = pfs_get_idx_of_inode_by_name(vfs, e, b, &err1);
  if (err1 == 0x114514) {
    if (err) { *err = 0x114514; }
    return;
  }
  *idx        = i;
  *dict_block = b;
}
void pfs_delete_name_link(vfs_t *vfs, u32 next, u32 dict_block) {
  while (next) {
    pfs_inode i;
    i      = pfs_get_inode_by_index(vfs, next, dict_block);
    i.type = 0;
    u32 n  = i.next;
    i.next = 0;
    pfs_set_inode_by_index(vfs, next, dict_block, &i);
    next = n;
  }
}
void pfs_rename(vfs_t *vfs, char *old_name, char *new_name, u32 dict_block) {
  u32 err, idx;
  idx = pfs_get_idx_of_inode_by_name(vfs, old_name, dict_block, &err);
  if (err == 0x114514) { return; }
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, dict_block);
  u32       d, t, s;
  d = i.dat;
  t = i.type;
  s = i.size;
  pfs_delete_name_link(vfs, i.next, dict_block);
  pfs_make_inode(vfs, idx, new_name, t, dict_block);
  i      = pfs_get_inode_by_index(vfs, idx, dict_block);
  i.dat  = d;
  i.size = s;
  pfs_set_inode_by_index(vfs, idx, dict_block, &i);
}
void init_pfs(vfs_t *vfs, pfs_t p) {
  vfs->cache = malloc(sizeof(pfs_t));
  *now_pfs_t = p;
  u8 mbr[512];
  now_pfs_t->read_block(now_pfs_t, 0, 1, mbr);
  pfs_mbr *mb = &mbr;
  // if (memcmp(mb->sign, "PFS\xff", 4) != 0) {
  //   free(now_pfs_t);
  //   now_pfs_t = NULL;
  //   return;
  // }
  now_pfs_t->first_sec_of_bitmap  = mb->first_sector_of_bitmap;
  now_pfs_t->resd_sec_end         = mb->resd_sector_end;
  now_pfs_t->resd_sec_start       = mb->resd_sector_start;
  now_pfs_t->root_dict_block      = 0;
  now_pfs_t->sec_bitmap_start     = mb->sec_bitmap_start;
  now_pfs_t->file_list            = NewList();
  now_pfs_t->bitmap               = NewList();
  now_pfs_t->prev_dict_block      = NewList();
  now_pfs_t->bitmap_buffer        = NewList();
  now_pfs_t->current_dict_block   = 0;
  now_pfs_t->current_bitmap_block = -1ll;
  now_pfs_t->bitmap_buff          = NULL;
  init_bitmap(vfs);
}

void pfs_InitFS(struct vfs_t *vfs, u8 disk_number) {
  pfs_t p;
  p.disk_number = disk_number;
  p.read_block  = pfs_read_block;
  p.write_block = pfs_write_block;
  init_pfs(vfs, p);
}
void pfs_CopyCache(struct vfs_t *dest, struct vfs_t *src) {
  dest->cache = malloc(sizeof(pfs_t));
  memcpy(dest->cache, src->cache, sizeof(pfs_t));
}
bool pfs_cd(struct vfs_t *vfs, char *dictName) {
  if (strcmp("..", dictName) == 0) {
    if (now_pfs_t->prev_dict_block->ctl->all != 0) {
      now_pfs_t->current_dict_block =
          FindForCount(now_pfs_t->prev_dict_block->ctl->all, now_pfs_t->prev_dict_block)->val;
      page_free((void *)FindForCount(vfs->path->ctl->all, vfs->path)->val, 255);
      DeleteVal(vfs->path->ctl->all, vfs->path);
      DeleteVal(now_pfs_t->prev_dict_block->ctl->all, now_pfs_t->prev_dict_block);
    }
    return true;
  } else if (strcmp(".", dictName) == 0) {
    return true;
  }
  u32 err;

  u32 new_dict_block =
      pfs_get_dict_block_by_name(vfs, dictName, now_pfs_t->current_dict_block, &err);
  if (err == 0x114514) { return false; }
  AddVal(now_pfs_t->current_dict_block, now_pfs_t->prev_dict_block);
  char *s = page_malloc(255);
  strcpy(s, dictName);
  AddVal((u32)s, vfs->path);
  now_pfs_t->current_dict_block = new_dict_block;
  return true;
}
bool pfs_ReadFile(struct vfs_t *vfs, char *path, char *buffer) {
  pfs_read_file(vfs, path, buffer, now_pfs_t->current_dict_block);
  return true;
}
bool pfs_WriteFile(struct vfs_t *vfs, char *path, char *buffer, int size) {
  pfs_write_file(vfs, path, size, buffer, now_pfs_t->current_dict_block);
}
List *pfs_ListFile(struct vfs_t *vfs, char *dictpath) {
  int            flags = 1;
  pfs_dict_block pdb;
  List          *result = NewList();
  u32            dict_block;
  if (strlen(dictpath) == 0) {
    dict_block = now_pfs_t->current_dict_block;
  } else {
    int err = 0;
    dict_block =
        pfs_get_dict_block_by_path(vfs, dictpath, NULL, now_pfs_t->current_dict_block, &err);
    if (err == 0x114514) { dict_block = 0; }
  }
  while (dict_block || flags) {
    memset(&pdb, 0, 512);
    now_pfs_t->read_block(now_pfs_t, block2sector(dict_block, now_pfs_t), 1, &pdb);
    for (int i = 0; i < 15; i++) {
      if (pdb.inodes[i].type == 3) {
        //        return;
        continue;
      }
      if (pdb.inodes[i].type != 0) {
        mstr *s = mstr_init();
        if (pdb.inodes[i].name[13] == 0) {
          mstr_add_str(s, pdb.inodes[i].name);
        } else {
          for (int j = 0; j < 13; j++) {
            mstr_add_char(s, pdb.inodes[i].name[j]);
          }
          u32 idx;
          idx = pdb.inodes[i].next;
          while (idx) {
            pfs_inode pi;
            pi                             = pfs_get_inode_by_index(vfs, idx, dict_block);
            pfs_inode_of_long_file_name *f = (pfs_inode_of_long_file_name *)&pi;
            if (f->name[26] == 0x0) {
              mstr_add_str(s, f->name);
              break;
            } else {
              for (int k = 0; k < 26; k++) {
                mstr_add_char(s, f->name[k]);
              }
            }
            idx = f->next;
          }
        }
        vfs_file *f = malloc(sizeof(vfs_file));
        strcpy(f->name, mstr_get(s));
        f->day    = 114514;
        f->hour   = 114514;
        f->minute = 114514;
        f->month  = 114514;
        f->size   = pdb.inodes[i].size;
        f->type   = pdb.inodes[i].type == 2 ? DIR : FLE;
        f->year   = 114514;
        AddVal((u32)f, result);
        mstr_free(s);
      }
    }
    dict_block = pdb.next;
    flags      = 0;
  }
  return result;
}
bool pfs_RenameFile(struct vfs_t *vfs, char *filename, char *filename_of_new) {
  pfs_rename(vfs, filename, filename_of_new, now_pfs_t->current_dict_block);
  return true;
}
bool pfs_CreateFile(struct vfs_t *vfs, char *filename) {
  char *e;
  u32   err   = 0;
  u32   block = pfs_get_dict_block_by_path(vfs, filename, &e, now_pfs_t->current_dict_block, &err);
  if (err == 0x114514) { return false; }
  if (*e) {
    pfs_create_file(vfs, e, block);
  } else {
    return false;
  }
}
void pfs_DeleteFs(struct vfs_t *vfs) {
  List *l;
  for (int i = 1; l = FindForCount(i, now_pfs_t->bitmap_buffer); i++) {
    free((void *)l->val);
  }
  DeleteList(now_pfs_t->bitmap_buffer);
  DeleteList(now_pfs_t->bitmap);
  DeleteList(now_pfs_t->file_list);
  DeleteList(now_pfs_t->prev_dict_block);
  free(vfs->cache);
}
bool pfs_Check(u8 disk_number) {
  u8 mbr[512];
  Disk_Read(0, 1, mbr, disk_number);
  pfs_mbr *mb = &mbr;
  if (memcmp(mb->sign, "PFS\xff", 4) != 0) { return false; }
  return true;
}
bool pfs_DelFile(struct vfs_t *vfs, char *path) {
  u32   b, err = 0;
  char *e;
  b = pfs_get_dict_block_by_path(vfs, path, &e, now_pfs_t->current_dict_block, &err);
  if (err == 0x114514) { return false; }
  pfs_delete_file(vfs, e, b);
}
bool pfs_DelDict(struct vfs_t *vfs, char *path) {
  // TODO:没写完
  pfs_delete_dict(vfs, path, now_pfs_t->current_dict_block);
  return true;
}
int pfs_FileSize(struct vfs_t *vfs, char *filename) {
  u32   b, err = 0;
  char *e;
  b = pfs_get_dict_block_by_path(vfs, filename, &e, now_pfs_t->current_dict_block, &err);
  if (err == 0x114514) { return -1; }
  u32 r = pfs_get_filesize(vfs, e, b, &err);
  if (err == 0x114514) { return -1; }
  return r;
}
bool pfs_Format(u8 disk_number) {
  pfs_t p;
  p.resd_sec_start      = 1;
  p.resd_sec_end        = 145;
  p.sec_bitmap_start    = 145;
  p.first_sec_of_bitmap = 146;
  p.read_block          = pfs_read_block;
  p.write_block         = pfs_write_block;
  p.disk_number         = disk_number;
  char vol[16]          = "POWERINTDOS386";
  pfs_format(p, vol);
}
bool pfs_CreateDict(struct vfs_t *vfs, char *filename) {
  char *e;
  u32   err   = 0;
  u32   block = pfs_get_dict_block_by_path(vfs, filename, &e, now_pfs_t->current_dict_block, &err);
  if (err == 0x114514) { return false; }
  if (*e) {
    pfs_create_dict(vfs, e, block);
  } else {
    return false;
  }
}
bool pfs_Attrib(struct vfs_t *vfs, char *filename, ftype type) {
  printf("Sorry, pfs does not support attrib at this time.\n");
  return false;
}
vfs_file *pfs_FileInfo(struct vfs_t *vfs, char *filename) {
  vfs_file *result = (vfs_file *)malloc(sizeof(vfs_file));
  u32       idx, b, err = 0;
  pfs_get_file_index_by_path(vfs, filename, now_pfs_t->current_dict_block, &err, &idx, &b);
  if (err == 0x114514) {
    free(result);
    return NULL;
  }
  pfs_inode i = pfs_get_inode_by_index(vfs, idx, b);
  strcpy(result->name, filename);
  result->day    = 114514;
  result->hour   = 114514;
  result->minute = 114514;
  result->month  = 114514;
  result->size   = i.size;
  result->type   = i.type == 2 ? DIR : FLE;
  result->year   = 114514;
  return result;
}
void reg_pfs() {
  vfs_t fs;
  fs.flag  = 1;
  fs.cache = NULL;
  strcpy(fs.FSName, "PFS");
  fs.CopyCache  = pfs_CopyCache;
  fs.Format     = pfs_Format;
  fs.CreateFile = pfs_CreateFile;
  fs.CreateDict = pfs_CreateDict;
  fs.DelDict    = pfs_DelDict;
  fs.DelFile    = pfs_DelFile;
  fs.ReadFile   = pfs_ReadFile;
  fs.WriteFile  = pfs_WriteFile;
  fs.DeleteFs   = pfs_DeleteFs;
  fs.cd         = pfs_cd;
  fs.FileSize   = pfs_FileSize;
  fs.Check      = pfs_Check;
  fs.ListFile   = pfs_ListFile;
  fs.InitFs     = pfs_InitFS;
  fs.RenameFile = pfs_RenameFile;
  fs.Attrib     = pfs_Attrib;
  fs.FileInfo   = pfs_FileInfo;
  vfs_register_fs(fs);
}
