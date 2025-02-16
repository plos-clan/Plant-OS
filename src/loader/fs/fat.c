// fat.c : fat文件系统的实现
#include <loader.h>

#pragma GCC optimize("O0")

bool Fat_WriteFile(struct vfs_t *vfs, char *path, char *buffer, int size);

finline int get_fat_date(u16 year, u16 month, u16 day) {
  year     -= 1980;
  u16 date  = 0;
  date     |= (year & 0x7f) << 9;
  date     |= (month & 0x0f) << 5;
  date     |= (day & 0x1f);
  return date;
}

finline int get_fat_time(u16 hour, u16 minute) {
  u16 time  = 0;
  time     |= (hour & 0x1f) << 11;
  time     |= (minute & 0x3f) << 5;
  return time;
}

void read_fat(u8 *img, int *fat, u8 *ff, int max, int type) {
  if (type == 12) {
    for (int i = 0, j = 0; i < max; i += 2) {
      fat[i + 0]  = (img[j + 0] | img[j + 1] << 8) & 0xfff;
      fat[i + 1]  = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
      j          += 3;
    }
    // 保留簇
    ff[0] = true;
    ff[1] = true;
    for (int i = 1; i < max; i++) {
      if (fat[i] > 0 && fat[i] < 0xff0) {
        ff[fat[i]] = true;
      } else if (fat[i] >= 0xff0 && fat[i] <= 0xfff) {
        ff[i + 1] = true;
      }
    }
  } else if (type == 16) {
    u16 *p = (u16 *)img;
    for (int i = 0; i != max; i++) {
      fat[i] = p[i];
    }
    ff[0] = true;
    ff[1] = true;
    for (int i = 1; i < max; i++) {
      if (fat[i] > 0 && fat[i] < 0xfff0) {
        ff[fat[i]] = true;
      } else if (fat[i] >= 0xfff0 && fat[i] <= 0xffff) {
        ff[i + 1] = true;
      }
    }
  } else if (type == 32) {
    u32 *p = (u32 *)img;
    for (int i = 0; i != max; i++) {
      fat[i] = p[i];
    }
    ff[0] = true;
    ff[1] = true;
    for (int i = 1; i < max; i++) {
      if (fat[i] > 0 && fat[i] < 0xffffff0) {
        ff[fat[i]] = true;
      } else if (fat[i] >= 0xffffff0 && fat[i] <= 0xfffffff) {
        ff[i + 1] = true;
      }
    }
  }
}

int get_directory_max(struct FAT_FILEINFO *directory, vfs_t *vfs) {
  if (directory == get_dm(vfs).root_directory) { return get_dm(vfs).RootMaxFiles; }
  for (int i = 1; FindForCount(i, (List *)get_dm(vfs).directory_list) != NULL; i++) {
    struct List *l = FindForCount(i, (List *)get_dm(vfs).directory_list);
    if ((struct FAT_FILEINFO *)l->value == directory) {
      return (int)FindForCount(i, (List *)get_dm(vfs).directory_max_list)->value;
    }
  }
  return -1;
}

void file_loadfile(int clustno, int size, char *buf, int *fat, vfs_t *vfs) {
  if (!size) { return; }
  void *img = page_alloc(((size - 1) / get_dm(vfs).ClustnoBytes + 1) * get_dm(vfs).ClustnoBytes);
  for (int i = 0; i != (size - 1) / get_dm(vfs).ClustnoBytes + 1; i++) {
    u32 sec = (get_dm(vfs).FileDataAddress + (clustno - 2) * get_dm(vfs).ClustnoBytes) /
              get_dm(vfs).SectorBytes;
    vdisk_read(sec, get_dm(vfs).ClustnoBytes / get_dm(vfs).SectorBytes,
               img + i * get_dm(vfs).ClustnoBytes, vfs->disk_number);
    clustno = fat[clustno];
  }
  memcpy((void *)buf, img, size);
  page_free(img, ((size - 1) / get_dm(vfs).SectorBytes + 1) * get_dm(vfs).SectorBytes);
  return;
}

void file_savefile(int clustno, int size, char *buf, int *fat, u8 *ff, vfs_t *vfs) {
  u32 clustall = 0;
  int tmp      = clustno;
  int end      = clustno_end(get_dm(vfs).type);
  while (fat[clustno] != end) { // 计算文件占多少Fat项 Fat项 = 大小 / 簇大小 + 1
    clustno = fat[clustno];
    clustall++;
  }
  int old_clustno = clustno;
  clustno         = tmp;
  int alloc_size;
  if (size > (clustall + 1) * get_dm(vfs).ClustnoBytes) { // 新大小 > (旧大小 / 簇大小 + 1) * 簇大小
    // 请求内存大小 = (新大小 / 簇大小 + 1) * 簇大小
    alloc_size = ((size - 1) / get_dm(vfs).ClustnoBytes + 1) * get_dm(vfs).ClustnoBytes;
    // 分配Fat（这里需要在写盘前分配）
    for (int size1  = size; size1 > ((clustall + 1) * get_dm(vfs).ClustnoBytes);
         size1     -= get_dm(vfs).ClustnoBytes) {
      for (int i = 0; i != get_dm(vfs).FatMaxTerms; i++) {
        if (!ff[i]) {
          fat[old_clustno] = i;
          old_clustno      = i;
          ff[i]            = true;
          break;
        }
      }
    }
    fat[old_clustno] = end; // 结尾Fat
    ff[old_clustno]  = true;
  } else if (size <= (clustall + 1) * get_dm(vfs).ClustnoBytes) { // 新大小 <= (旧大小 / 簇大小
                                                                  // + 1) * 簇大小
    // 请求内存大小 = (旧大小 / 簇大小 + 1) * 簇大小
    alloc_size = (clustall + 1) * get_dm(vfs).ClustnoBytes;
    // 这里不分配Fat的原因是要清空更改后多余的数据
  }
  void *img = page_alloc(alloc_size);
  bzero((char *)img, alloc_size);
  memcpy(img, buf, size); // 把要写入的数据复制到新请求的内存地址
  for (int i = 0; i != (alloc_size / get_dm(vfs).ClustnoBytes); i++) {
    u32 sec = (get_dm(vfs).FileDataAddress + (clustno - 2) * get_dm(vfs).ClustnoBytes) /
              get_dm(vfs).SectorBytes;
    vdisk_write(sec, get_dm(vfs).ClustnoBytes / get_dm(vfs).SectorBytes,
                img + i * get_dm(vfs).ClustnoBytes, vfs->disk_number);
    clustno = fat[clustno];
  }
  page_free(img, alloc_size);
  if (size < clustall * get_dm(vfs).ClustnoBytes) { // 新大小 < (旧大小 / 簇大小) * 簇大小
    // 分配Fat（中间情况没必要分配）
    int i = old_clustno;
    for (int size1  = clustall * get_dm(vfs).ClustnoBytes; size1 > size;
         size1     -= get_dm(vfs).ClustnoBytes) {
      fat[i] = 0;
      ff[i]  = false;
      for (int j = 0; j != get_dm(vfs).FatMaxTerms; j++) {
        if (fat[j] == i) { i = j; }
      }
    }
    old_clustno      = i;
    fat[old_clustno] = end;
    ff[old_clustno]  = true;
  }
  file_savefat(fat, 0, get_dm(vfs).FatMaxTerms, vfs);
}
void file_saveinfo(struct FAT_FILEINFO *directory, vfs_t *vfs) {
  if (directory == get_dm(vfs).root_directory) {
    vdisk_write(get_dm(vfs).RootDictAddress / get_dm(vfs).SectorBytes,
                get_dm(vfs).RootMaxFiles * 32 / get_dm(vfs).SectorBytes, (void *)directory,
                vfs->disk_number);
  } else {
    for (int i = 1; FindForCount(i, (List *)get_dm(vfs).directory_list) != NULL; i++) {
      struct List *list = FindForCount(i, (List *)get_dm(vfs).directory_list);
      if ((void *)list->value == (void *)directory) {
        list  = FindForCount(i, (List *)get_dm(vfs).directory_clustno_list);
        int k = (int)FindForCount(i, (List *)get_dm(vfs).directory_max_list)->value * 32 /
                get_dm(vfs).ClustnoBytes;
        for (int j = list->value, l = 0; l != k; l++) {
          vdisk_write((get_dm(vfs).FileDataAddress + (j - 2) * get_dm(vfs).ClustnoBytes) /
                          get_dm(vfs).SectorBytes,
                      get_dm(vfs).ClustnoBytes / get_dm(vfs).SectorBytes,
                      (char *)directory + l * get_dm(vfs).SectorBytes, vfs->disk_number);
          j = get_dm(vfs).fat[j];
        }
        break;
      }
    }
  }
}
void file_savefat(int *fat, int clustno, int length, vfs_t *vfs) {
  u8 *img = (u8 *)(get_dm(vfs).ADR_DISKIMG + get_dm(vfs).Fat1Address);
  int size, sec;
  if (get_dm(vfs).type == 12) {
    for (int i = 0; i <= length; i++) {
      if ((clustno + i) % 2 == 0) {
        img[(clustno + i) * 3 / 2 + 0] = fat[clustno + i] & 0xff;
        img[(clustno + i) * 3 / 2 + 1] =
            (fat[clustno + i] >> 8 | (img[(clustno + i) * 3 / 2 + 1] & 0xf0)) & 0xff;
      } else if ((clustno + i) % 2 != 0) {
        img[(clustno + i - 1) * 3 / 2 + 1] =
            ((img[(clustno + i - 1) * 3 / 2 + 1] & 0x0f) | fat[clustno + i] << 4) & 0xff;
        img[(clustno + i - 1) * 3 / 2 + 2] = (fat[clustno + i] >> 4) & 0xff;
      }
    }
    size = length * 3 / 2 - 1;
    sec  = clustno * 3 / 2;
  } else if (get_dm(vfs).type == 16) {
    for (int i = 0; i <= length; i++) {
      img[(clustno + i) * 2 + 0] = fat[clustno + i] & 0xff;
      img[(clustno + i) * 2 + 1] = (fat[clustno + i] >> 8) & 0xff;
    }
    size = length * 2 - 1;
    sec  = clustno * 2;
  } else if (get_dm(vfs).type == 32) {
    for (int i = 0; i <= length; i++) {
      img[(clustno + i) * 4 + 0] = fat[clustno + i] & 0xff;
      img[(clustno + i) * 4 + 1] = (fat[clustno + i] >> 8) & 0xff;
      img[(clustno + i) * 4 + 2] = (fat[clustno + i] >> 16) & 0xff;
      img[(clustno + i) * 4 + 3] = fat[clustno + i] >> 24;
    }
    size = length * 4 - 1;
    sec  = clustno * 4;
  }
  vdisk_write((get_dm(vfs).Fat1Address + sec) / get_dm(vfs).SectorBytes,
              size / get_dm(vfs).SectorBytes + 1,
              (void *)(get_dm(vfs).ADR_DISKIMG + get_dm(vfs).Fat1Address), vfs->disk_number);
  vdisk_write((get_dm(vfs).Fat2Address + sec) / get_dm(vfs).SectorBytes,
              size / get_dm(vfs).SectorBytes + 1,
              (void *)(get_dm(vfs).ADR_DISKIMG + get_dm(vfs).Fat2Address), vfs->disk_number);
}
struct FAT_FILEINFO *file_search(char *name, struct FAT_FILEINFO *finfo, int max) {
  int  i, j;
  char s[12];
  for (j = 0; j < 11; j++) {
    s[j] = ' ';
  }
  j = 0;
  for (i = 0; name[i] != 0; i++) {
    if (j >= 11) { return 0; /*没有找到*/ }
    if (name[i] == '.' && j <= 8) {
      j = 8;
    } else {
      s[j] = name[i];
      if ('a' <= s[j] && s[j] <= 'z') {
        /*将小写字母转换为大写字母*/
        s[j] -= 0x20;
      }
      j++;
    }
  }
  for (i = 0; i < max;) {
    if (finfo[i].name[0] == 0x00) { break; }
    if ((finfo[i].type & 0x18) == 0) {
      for (j = 0; j < 11; j++) {
        if (finfo[i].name[j] != s[j]) { goto next; }
      }
      return finfo + i; /*找到文件*/
    }
  next:
    i++;
  }
  return 0; /*没有找到*/
}
struct FAT_FILEINFO *dict_search(char *name, struct FAT_FILEINFO *finfo, int max) {
  int  i, j;
  char s[12];
  for (j = 0; j < 11; j++) {
    s[j] = ' ';
  }
  j = 0;
  for (i = 0; name[i] != 0; i++) {
    if (j >= 11) {
      return 0; /*没有找到*/
    } else {
      s[j] = name[i];
      if ('a' <= s[j] && s[j] <= 'z') {
        /*将小写字母转换为大写字母*/
        s[j] -= 0x20;
      }
      j++;
    }
  }
  for (i = 0; i < max;) {
    if (finfo[i].name[0] == 0x00) { break; }
    if (finfo[i].type == 0x10) {
      for (j = 0; j < 11; j++) {
        if (finfo[i].name[j] != s[j]) { goto next; }
      }
      return finfo + i; /* 找到文件 */
    }
  next:
    i++;
  }
  return 0; /*没有找到*/
}
struct FAT_FILEINFO *Get_File_Address(char *path1, vfs_t *vfs) {
  struct FAT_FILEINFO *bmpDict      = get_now_dir(vfs);
  int                  drive_number = vfs->disk_number;
  char                *path         = (char *)page_alloc(strlen(path1) + 1);
  char                *bmp          = path;
  strcpy(path, path1);
  strupper(path);
  if (strncmp("/", path, 1) == 0) {
    path    += 1;
    bmpDict  = get_dm(vfs).root_directory;
  }
  if (path[0] == '\\' || path[0] == '/') {
    //跳过反斜杠和正斜杠
    for (int i = 0; i < strlen(path); i++) {
      if (path[i] != '\\' && path[i] != '/') {
        path += i;
        break;
      }
    }
  }
  char                *temp_name = (char *)page_alloc(128);
  struct FAT_FILEINFO *finfo     = get_dm(vfs).root_directory;
  int                  i         = 0;
  while (1) {
    int j;
    for (j = 0; i < strlen(path); i++, j++) {
      if (path[i] == '\\' || path[i] == '/') {
        temp_name[j] = '\0';
        i++;
        break;
      }
      temp_name[j] = path[i];
    }
    finfo = dict_search(temp_name, bmpDict, get_directory_max(bmpDict, vfs));
    if (finfo == 0) {
      if (path[i] != '\0') {
        page_free((void *)temp_name, 128);
        page_free((void *)bmp, strlen(path1) + 1);
        return 0;
      }
      finfo = file_search(temp_name, bmpDict, get_directory_max(bmpDict, vfs));
      if (finfo == 0) {
        page_free((void *)temp_name, 128);
        page_free((void *)bmp, strlen(path1) + 1);
        return 0;
      } else {
        goto END;
      }
    } else {
      if (get_clustno(finfo->clustno_high, finfo->clustno_low) != 0) {
        for (int count = 1; FindForCount(count, (List *)get_dm(vfs).directory_clustno_list) != NULL;
             count++) {
          struct List *list = FindForCount(count, (List *)get_dm(vfs).directory_clustno_list);
          if (get_clustno(finfo->clustno_high, finfo->clustno_low) == list->value) {
            list    = FindForCount(count, (List *)get_dm(vfs).directory_list);
            bmpDict = (struct FAT_FILEINFO *)list->value;
            break;
          }
        }
      } else {
        bmpDict = get_dm(vfs).root_directory;
      }
      bzero(temp_name, 128);
    }
  }
END:
  page_free((void *)temp_name, 128);
  page_free((void *)bmp, strlen(path1) + 1);
  return finfo;
}
struct FAT_FILEINFO *Get_dictaddr(char *path1, vfs_t *vfs) {
  // TODO: Modifly it
  struct FAT_FILEINFO *bmpDict      = get_now_dir(vfs);
  int                  drive_number = vfs->disk_number;
  char                *path         = (char *)page_alloc(strlen(path1) + 1);
  char                *bmp          = path;
  strcpy(path, path1);
  strupper(path);
  if (strncmp("/", path, 1) == 0) {
    path    += 1;
    bmpDict  = get_dm(vfs).root_directory;
  }
  if (path[0] == '\\' || path[0] == '/') {
    //跳过反斜杠和正斜杠
    for (int i = 0; i < strlen(path); i++) {
      if (path[i] != '\\' && path[i] != '/') {
        path += i;
        break;
      }
    }
  }
  char                *temp_name = (char *)page_alloc(128);
  struct FAT_FILEINFO *finfo;
  int                  i = 0;
  while (1) {
    int j;
    for (j = 0; i < strlen(path); i++, j++) {
      if (path[i] == '\\' || path[i] == '/') {
        temp_name[j] = '\0';
        i++;
        break;
      }
      temp_name[j] = path[i];
    }
    finfo = dict_search(temp_name, bmpDict, get_directory_max(bmpDict, vfs));
    if (finfo == 0) {
      goto END;
    } else {
      if (get_clustno(finfo->clustno_high, finfo->clustno_low) != 0) {
        for (int count = 1; FindForCount(count, (List *)get_dm(vfs).directory_clustno_list) != NULL;
             count++) {
          struct List *list = FindForCount(count, (List *)get_dm(vfs).directory_clustno_list);
          if (get_clustno(finfo->clustno_high, finfo->clustno_low) == list->value) {
            list    = FindForCount(count, (List *)get_dm(vfs).directory_list);
            bmpDict = (struct FAT_FILEINFO *)list->value;
            break;
          }
        }
      } else {
        bmpDict = get_dm(vfs).root_directory;
      }
      bzero(temp_name, 128);
      if (path[i] == '\0') { goto END; }
    }
  }
END:
  page_free((void *)temp_name, 128);
  page_free((void *)bmp, strlen(path1) + 1);
  return bmpDict;
}
void mkdir(char *dictname, int last_clust, vfs_t *vfs) {
  /*
          dictname:目录名
          last_clust:上一级目录的簇号
  */
  mkfile(dictname, vfs);
  // 三个目录项（模板）
  static struct FAT_FILEINFO model[3];
  // .目录项，指向自己
  // ..目录项，指向上一级目录
  // 空目录项（为mkfile函数提供指引）
  model[0].reserve = 0;
  model[1].reserve = 0;
  model[2].reserve = 0;
  memcpy(model[2].name, "NULL       ", 11);
  model[2].type = 0x20; // 文件的type属性是0x20
  // 将size date time这类属性全部设置为0
  model[2].size        = 0;
  model[2].update_date = get_fat_date(get_year(), get_mon_hex(), get_day_of_month());
  model[2].update_time = get_fat_time(get_hour_hex(), get_min_hex());
  for (int i = 0; i != get_dm(vfs).FatMaxTerms; i++) {
    if (!get_dm(vfs).fat[i]) {
      model[2].clustno_low           = i;
      int end                        = clustno_end(get_dm(vfs).type);
      get_dm(vfs).fat[i]             = end;
      get_dm(vfs).FatClustnoFlags[i] = true;
      break;
    }
  }
  model[0].name[0] = '.';
  for (int i = 1; i != 8; i++) {
    model[0].name[i] = ' ';
  }
  for (int i = 0; i != 3; i++) {
    model[0].ext[i] = ' ';
  }
  struct FAT_FILEINFO *finfo1 = Get_File_Address(dictname, vfs);
  model[0].type               = 0x10;
  model[0].clustno_low        = finfo1->clustno_low; // 指向自己
  model[0].clustno_high       = finfo1->clustno_high;
  model[0].size               = 0;
  model[0].update_date        = get_fat_date(get_year(), get_mon_hex(), get_day_of_month());
  model[0].update_time        = get_fat_time(get_hour_hex(), get_min_hex());
  model[1].name[0]            = '.';
  model[1].name[1]            = '.';
  for (int i = 2; i != 8; i++) {
    model[1].name[i] = ' ';
  }
  for (int i = 0; i != 3; i++) {
    model[1].ext[i] = ' ';
  }
  model[1].clustno_low = last_clust;
  model[1].size        = 0;
  model[1].update_date = get_fat_date(get_year(), get_mon_hex(), get_day_of_month());
  model[1].update_time = get_fat_time(get_hour_hex(), get_min_hex());
  model[1].type        = 0x10;
  Fat_WriteFile(vfs, dictname, (char *)&model[0], 32 * 3);
  struct FAT_FILEINFO *finfo = Get_File_Address(dictname, vfs);
  finfo->type                = 0x10; // 是目录（文件夹的type属性是0x10）
  finfo->size                = 0;
  int drive_number           = vfs->disk_number;
  AddVal(get_clustno(finfo->clustno_high, finfo->clustno_low),
         (List *)get_dm(vfs).directory_clustno_list);
  if (last_clust == 0) {
    file_saveinfo(get_dm(vfs).root_directory, vfs);
  } else {
    for (int i = 1; FindForCount(i, (List *)get_dm(vfs).directory_clustno_list) != NULL; i++) {
      struct List *list = FindForCount(i, (List *)get_dm(vfs).directory_clustno_list);
      if (list->value == last_clust) {
        list                         = FindForCount(i, (List *)get_dm(vfs).directory_list);
        struct FAT_FILEINFO *d_finfo = (struct FAT_FILEINFO *)list->value;
        file_saveinfo(d_finfo, vfs);
      }
    }
  }
  file_savefat(get_dm(vfs).fat, get_clustno(model[2].clustno_high, model[2].clustno_low), 1, vfs);

  void *directory_alloc = page_alloc(get_dm(vfs).ClustnoBytes);
  vdisk_read(
      (get_dm(vfs).FileDataAddress +
       (get_clustno(finfo->clustno_high, finfo->clustno_low) - 2) * get_dm(vfs).ClustnoBytes) /
          get_dm(vfs).SectorBytes,
      get_dm(vfs).ClustnoBytes / get_dm(vfs).SectorBytes, directory_alloc, vfs->disk_number);
  AddVal((u32)directory_alloc, (List *)get_dm(vfs).directory_list);
  AddVal(get_dm(vfs).ClustnoBytes / 32, (List *)get_dm(vfs).directory_max_list);
  return;
}

struct FAT_FILEINFO *clust_sech(int clustno, struct FAT_FILEINFO *finfo, int max) {
  //通过簇号找到文件信息
  int i, j;
  j = 0;
  for (i = 0; i < max; i++) {
    if (finfo[i].clustno_low == clustno) { return finfo + i; }
  }
  return 0; /*没找到*/
}
int del(char *cmdline, vfs_t *vfs) {
  //删除某个文件
  char                *name = cmdline;
  int                  i;
  struct FAT_FILEINFO *finfo;
  finfo = Get_File_Address(name, vfs);
  if (finfo == 0 || finfo->type == 0x01 || finfo->type == 0x04) { return 0; }
  file_savefile(get_clustno(finfo->clustno_high, finfo->clustno_low), 0, NULL, get_dm(vfs).fat,
                get_dm(vfs).FatClustnoFlags, vfs);
  finfo->name[0]                                                                    = 0xe5;
  get_dm(vfs).fat[get_clustno(finfo->clustno_high, finfo->clustno_low)]             = 0;
  get_dm(vfs).FatClustnoFlags[get_clustno(finfo->clustno_high, finfo->clustno_low)] = false;
  file_saveinfo(Get_dictaddr(name, vfs), vfs);
  file_savefat(get_dm(vfs).fat, get_clustno(finfo->clustno_high, finfo->clustno_low), 1, vfs);
  return 1;
}
int deldir(char *path, vfs_t *vfs) {
  struct FAT_FILEINFO *finfo = Get_dictaddr(path, vfs);
  if (finfo == Get_dictaddr(".", vfs)) { return 0; }
  struct FAT_FILEINFO *f = get_now_dir(vfs);
  get_now_dir(vfs)       = finfo;
  for (int i = 2; finfo[i].name[0] != '\0'; i++) {
    if (finfo[i].type == 0x10 && finfo[i].name[0] != 0xe5) {
      char s[30];
      int  j = 0;
      for (; finfo[i].name[j] != ' '; j++) {
        s[j] = finfo[i].name[j];
      }
      s[j] = '\0';
      // printf("(CALL)DEL DIR:%s\n", s);
      if (deldir(s, vfs) == -1) { return 0; }
      // return -1;
    }
  }
  for (int i = 2; finfo[i].name[0] != '\0'; i++) {
    if (finfo[i].name[0] != 0xe5 && finfo[i].type != 0x10) {
      char s[30];
      int  p = 0;
      for (int j = 0; finfo[i].name[j] != ' '; j++, p++) {
        s[p] = finfo[i].name[j];
      }
      if (finfo[i].ext[0] != ' ') {
        s[p++] = '.';
        for (int j = 0; finfo[i].ext[j] != ' ' || j != 3; j++, p++) {
          s[p] = finfo[i].ext[j];
        }
      }
      s[p] = '\0';
      // printf("(IN)DEL FILE:%s\n", s);
      if (del(s, vfs) == -1) { return 0; }
    }
  }
  get_now_dir(vfs) = f;
  // printf("(IN)DEL SELF\n");
  struct FAT_FILEINFO *root_finfo;
  if (finfo[1].clustno_low == 0) {
    root_finfo = get_dm(vfs).root_directory;
  } else {
    for (int i = 1; FindForCount(i, (List *)get_dm(vfs).directory_clustno_list) != NULL; i++) {
      if (FindForCount(i, (List *)get_dm(vfs).directory_clustno_list)->value ==
          finfo[1].clustno_low) {
        root_finfo =
            (struct FAT_FILEINFO *)FindForCount(i, (List *)get_dm(vfs).directory_list)->value;
        // printf("FIND ROOT %08x\n", root_finfo);
      }
    }
  }
  for (int i = 0; root_finfo[i].name[0] != '\0'; i++) {
    // printf("ROOT FILE:%s\n", root_finfo[i].name);
    if (root_finfo[i].clustno_low == finfo[0].clustno_low) {
      root_finfo[i].name[0] = 0xe5;
      break;
    }
  }
  get_dm(vfs).fat[get_clustno(finfo->clustno_high, finfo->clustno_low)]             = 0;
  get_dm(vfs).FatClustnoFlags[get_clustno(finfo->clustno_high, finfo->clustno_low)] = false;
  file_saveinfo(Get_dictaddr(path, vfs), vfs);
  file_savefat(get_dm(vfs).fat, get_clustno(finfo->clustno_high, finfo->clustno_low), 1, vfs);
  return 1;
}
void mkfile(char *name, vfs_t *vfs) {
  char                 s[12];
  int                  i, j;
  struct FAT_FILEINFO *finfo = Get_dictaddr(name, vfs);
  int                  max   = get_directory_max(finfo, vfs);

  char *path = name;
  for (i = strlen(name); i >= 0; i--) {
    if (name[i] == '/' || name[i] == '\\') {
      name += i + 1;
      break;
    }
  }

  for (j = 0; j != 12; j++) {
    s[j] = ' ';
  }
  j = 0;
  for (i = 0; name[i] != 0; i++) {
    if (j >= 11) { return; }
    if (name[i] == '.' && j <= 8) {
      j = 8;
    } else {
      s[j] = name[i];
      if ('a' <= s[j] && s[j] <= 'z') { s[j] -= 0x20; }
      j++;
    }
  }
  for (i = 0;; i++) {
    if (finfo[i].name[0] == 0x00 || finfo[i].name[0] == 0xe5) {
      finfo = finfo + i;
      break;
    }
    if (i >= max && finfo != get_dm(vfs).root_directory) {
      for (int j = 1; FindForCount(j, (List *)get_dm(vfs).directory_list) != NULL; j++) {
        struct List *l = FindForCount(j, (List *)get_dm(vfs).directory_list);
        if ((struct FAT_FILEINFO *)l->value == finfo) {
          max += get_dm(vfs).ClustnoBytes / 32;
          FindForCount(j, (List *)get_dm(vfs).directory_max_list)->value = max;
          struct FAT_FILEINFO *finfo_ = (struct FAT_FILEINFO *)realloc((void *)finfo, max * 32);
          if (get_now_dir(vfs) == finfo) { get_now_dir(vfs) = finfo_; }
          finfo    = finfo_;
          l->value = (uintptr_t)finfo;
          break;
        }
      }
      int end = clustno_end(get_dm(vfs).type), last;
      for (int j = get_clustno(finfo->clustno_high, finfo->clustno_low);; j = get_dm(vfs).fat[j]) {
        if (get_dm(vfs).fat[j] == end) {
          last = j;
          break;
        }
      }
      for (int j = 0; j != get_dm(vfs).FatMaxTerms; j++) {
        if (!get_dm(vfs).fat[j]) {
          get_dm(vfs).fat[last]          = j;
          get_dm(vfs).fat[j]             = end;
          get_dm(vfs).FatClustnoFlags[j] = true;
          break;
        }
      }
      i--;
    }
  }
  for (i = 0; i != 8; i++) {
    finfo->name[i] = s[i];
  }
  for (i = 8; i != 11; i++) {
    finfo->ext[i - 8] = s[i];
  }
  finfo->type = 0x20;
  for (int i = 0; i != get_dm(vfs).FatMaxTerms; i++) {
    if (!get_dm(vfs).fat[i]) {
      finfo->clustno_low             = i & 0xffff;
      finfo->clustno_high            = i >> 16;
      int end                        = clustno_end(get_dm(vfs).type);
      get_dm(vfs).fat[i]             = end;
      get_dm(vfs).FatClustnoFlags[i] = true;
      break;
    }
  }
  finfo->reserve     = 0;
  finfo->update_time = get_fat_time(get_hour_hex(), get_min_hex());
  finfo->update_date = get_fat_date(get_year(), get_mon_hex(), get_day_of_month());
  finfo->size        = 0;
  file_saveinfo(Get_dictaddr(path, vfs), vfs);
  file_savefat(get_dm(vfs).fat, get_clustno(finfo->clustno_high, finfo->clustno_low), 1, vfs);
}

int changedict(char *dictname, vfs_t *vfs) {
  // cd命令的依赖函数
  strupper(dictname);

  if (strcmp(dictname, "/") == 0) {
    while (vfs->path->ctl->all != 0) {
      page_free((void *)FindForCount(vfs->path->ctl->all, vfs->path)->value, 255);
      DeleteVal(vfs->path->ctl->all, vfs->path);
    }
    get_now_dir(vfs) = get_dm(vfs).root_directory;
    return 1;
  }
  struct FAT_FILEINFO *finfo =
      dict_search(dictname, get_now_dir(vfs), get_directory_max(get_now_dir(vfs), vfs));
  //找文件夹
  if (finfo == 0) {
    //没找到
    return 0;
  }
  if (get_clustno(finfo->clustno_high, finfo->clustno_low) == 0) {
    //根目录
    while (vfs->path->ctl->all != 0) {
      page_free((void *)(FindForCount(vfs->path->ctl->all, vfs->path)->value), 255);
      DeleteVal(vfs->path->ctl->all, vfs->path);
    }
    get_now_dir(vfs) = get_dm(vfs).root_directory;
    return 1;
  }
  //..进行特殊的处理
  //.不进行处理
  //其他按照下面的方式处理
  if (strcmp(dictname, "..") != 0 && strcmp(dictname, ".") != 0) {
    char *dict = page_alloc(255);
    strcpy(dict, dictname);
    AddVal((u32)dict, vfs->path);
  }

  if (strcmp(dictname, "..") == 0) {
    page_free((void *)FindForCount(vfs->path->ctl->all, vfs->path)->value, 255);
    DeleteVal(vfs->path->ctl->all, vfs->path);
  }
  for (int count = 1; FindForCount(count, (List *)get_dm(vfs).directory_clustno_list) != NULL;
       count++) {
    struct List *list = FindForCount(count, (List *)get_dm(vfs).directory_clustno_list);
    if (get_clustno(finfo->clustno_high, finfo->clustno_low) == list->value) {
      list             = FindForCount(count, (List *)get_dm(vfs).directory_list);
      get_now_dir(vfs) = (struct FAT_FILEINFO *)list->value;
      break;
    }
  }
  return 1;
}

int rename(char *src_name, char *dst_name, vfs_t *vfs) {
  strupper(src_name);
  strupper(dst_name);
  char name[9], ext[4];
  int  i;
  bzero(name, 9);
  bzero(ext, 4);
  for (i = 0; dst_name[i] != '.'; i++) {
    name[i] = dst_name[i];
  }
  i++;
  for (int j = 0; i != strlen(dst_name); i++, j++) {
    ext[j] = dst_name[i];
  }
  struct FAT_FILEINFO *finfo = Get_File_Address(src_name, vfs);
  if (finfo == 0 || finfo->type == 0x01 || finfo->type == 0x04) { return 0; }
  memset((void *)finfo->name, ' ', 11);
  for (i = 0; i != strlen(name); i++) {
    finfo->name[i] = name[i];
  }
  for (i = 0; i != strlen(ext); i++) {
    finfo->ext[i] = ext[i];
  }
  file_saveinfo(Get_dictaddr(src_name, vfs), vfs);
  return 1;
}

int format(int drive) {
  return 0;
}

int attrib(char *filename, ftype type, struct vfs_t *vfs) {
  struct FAT_FILEINFO *finfo = Get_File_Address(filename, vfs);
  if (finfo == 0) { return 0; }
  if (type == FLE)
    finfo->type = 0x20;
  else if (type == RDO)
    finfo->type = 0x01;
  else if (type == HID)
    finfo->type = 0x02;
  else if (type == SYS)
    finfo->type = 0x04;
  else
    return 0;
  file_saveinfo(Get_dictaddr(filename, vfs), vfs);
  return 1;
}

void fat_InitFS(struct vfs_t *vfs, u8 disk_number) {
  vfs->cache        = malloc(sizeof(fat_cache));
  void *boot_sector = page_alloc(512);
  vdisk_read(0, 1, boot_sector, disk_number);

  if (memcmp(boot_sector + BS_FileSysType, "FAT12   ", 8) == 0) {
    get_dm(vfs).type = 12;
  } else if (memcmp(boot_sector + BS_FileSysType, "FAT16   ", 8) == 0) {
    get_dm(vfs).type = 16;
  } else if (memcmp(boot_sector + BS_FileSysType + BPB_Fat32ExtByts, "FAT32   ", 8) == 0) {
    get_dm(vfs).type = 32;
  }
  get_dm(vfs).SectorBytes  = *(u16 *)(boot_sector + BPB_BytsPerSec);
  get_dm(vfs).RootMaxFiles = *(u16 *)(boot_sector + BPB_RootEntCnt);
  get_dm(vfs).ClustnoBytes = get_dm(vfs).SectorBytes * *(u8 *)(boot_sector + BPB_SecPerClus);
  get_dm(vfs).Fat1Address  = *(u16 *)(boot_sector + BPB_RsvdSecCnt) * get_dm(vfs).SectorBytes;
  if (get_dm(vfs).type != 32) {
    get_dm(vfs).RootDictAddress =
        (*(u8 *)(boot_sector + BPB_NumFATs) * *(u16 *)(boot_sector + BPB_FATSz16) +
         *(u16 *)(boot_sector + BPB_RsvdSecCnt)) *
        get_dm(vfs).SectorBytes;
    get_dm(vfs).FileDataAddress = get_dm(vfs).RootDictAddress + get_dm(vfs).RootMaxFiles * 32;
    if (*(u16 *)(boot_sector + BPB_TotSec16) != 0) {
      get_dm(vfs).imgTotalSize = *(u16 *)(boot_sector + BPB_TotSec16) * get_dm(vfs).SectorBytes;
    } else {
      get_dm(vfs).imgTotalSize = *(u32 *)(boot_sector + BPB_TotSec32) * get_dm(vfs).SectorBytes;
    }
    get_dm(vfs).Fat2Address =
        get_dm(vfs).Fat1Address + *(u16 *)(boot_sector + BPB_FATSz16) * get_dm(vfs).SectorBytes;
  } else {
    get_dm(vfs).FileDataAddress =
        (*(u8 *)(boot_sector + BPB_NumFATs) * *(u32 *)(boot_sector + BPB_FATSz32) +
         *(u16 *)(boot_sector + BPB_RsvdSecCnt)) *
        get_dm(vfs).SectorBytes;
    get_dm(vfs).RootDictAddress =
        get_dm(vfs).FileDataAddress +
        (*(u32 *)(boot_sector + BPB_RootClus) - 2) * get_dm(vfs).ClustnoBytes;
    get_dm(vfs).imgTotalSize = *(u32 *)(boot_sector + BPB_TotSec32) * get_dm(vfs).SectorBytes;
    get_dm(vfs).Fat2Address =
        get_dm(vfs).Fat1Address + *(u32 *)(boot_sector + BPB_FATSz32) * get_dm(vfs).SectorBytes;
    get_dm(vfs).RootMaxFiles = get_dm(vfs).ClustnoBytes / 32;
  }
  get_dm(vfs).FatMaxTerms =
      (get_dm(vfs).Fat2Address - get_dm(vfs).Fat1Address) * 8 / get_dm(vfs).type;

  u32 sec = (get_dm(vfs).RootDictAddress + get_dm(vfs).RootMaxFiles * 32) / get_dm(vfs).SectorBytes;
  get_dm(vfs).ADR_DISKIMG =
      (u32)malloc(get_dm(vfs).RootDictAddress + get_dm(vfs).RootMaxFiles * 32);

  vdisk_read(0, sec, (void *)get_dm(vfs).ADR_DISKIMG, disk_number);

  get_dm(vfs).fat             = malloc(get_dm(vfs).FatMaxTerms * sizeof(int));
  get_dm(vfs).FatClustnoFlags = malloc(get_dm(vfs).FatMaxTerms * sizeof(char));
  read_fat((u8 *)(get_dm(vfs).ADR_DISKIMG + (u32)get_dm(vfs).Fat1Address), get_dm(vfs).fat,
           get_dm(vfs).FatClustnoFlags, get_dm(vfs).FatMaxTerms, get_dm(vfs).type);
  get_dm(vfs).root_directory = (struct FAT_FILEINFO *)malloc(get_dm(vfs).RootMaxFiles * 32);
  memcpy((void *)get_dm(vfs).root_directory,
         (void *)get_dm(vfs).ADR_DISKIMG + get_dm(vfs).RootDictAddress,
         get_dm(vfs).RootMaxFiles * 32);
  get_dm(vfs).directory_list         = (struct LIST *)NewList();
  get_dm(vfs).directory_clustno_list = (struct LIST *)NewList();
  get_dm(vfs).directory_max_list     = (struct LIST *)NewList();
  struct FAT_FILEINFO *finfo         = get_dm(vfs).root_directory;

  for (int i = 0; i != get_dm(vfs).RootMaxFiles; i++) {
    if (finfo[i].type == 0x10 && finfo[i].name[0] != 0xe5) {
      AddVal(get_clustno(finfo[i].clustno_high, finfo[i].clustno_low),
             (struct List *)get_dm(vfs).directory_clustno_list);
      int end = clustno_end(get_dm(vfs).type), k = 1;
      for (int j                        = get_clustno(finfo[i].clustno_high, finfo[i].clustno_low);
           get_dm(vfs).fat[j] != end; j = get_dm(vfs).fat[j], k++)
        ;
      void *directory_alloc = malloc(k * get_dm(vfs).ClustnoBytes);
      for (int j = get_clustno(finfo[i].clustno_high, finfo[i].clustno_low), l = 0; l != k; l++) {
        u32 sec1 = (get_dm(vfs).FileDataAddress + (j - 2) * get_dm(vfs).ClustnoBytes) /
                   get_dm(vfs).SectorBytes;
        vdisk_read(sec1, get_dm(vfs).ClustnoBytes / get_dm(vfs).SectorBytes,
                   (char *)directory_alloc + l * get_dm(vfs).SectorBytes, disk_number);
        j = get_dm(vfs).fat[j];
      }
      AddVal((int)directory_alloc, (struct List *)get_dm(vfs).directory_list);
      AddVal(k * get_dm(vfs).ClustnoBytes / 32, (struct List *)get_dm(vfs).directory_max_list);
    }
    if (finfo[i].name[0] == 0) { break; }
  }

  for (int i = 1; FindForCount(i, (struct List *)get_dm(vfs).directory_list) != NULL; i++) {
    struct List *list = FindForCount(i, (struct List *)get_dm(vfs).directory_list);
    finfo             = (struct FAT_FILEINFO *)list->value;
    for (int j = 0; j != get_directory_max(finfo, vfs); j++) {
      if (finfo[j].type == 0x10 && finfo[j].name[0] != 0xe5 &&
          strncmp(".", (char *)finfo[j].name, 1) != 0 &&
          strncmp("..", (char *)finfo[j].name, 2) != 0) {
        AddVal(get_clustno(finfo[j].clustno_high, finfo[j].clustno_low),
               (struct List *)get_dm(vfs).directory_clustno_list);
        int end = clustno_end(get_dm(vfs).type), k = 1;
        for (int m = get_clustno(finfo[j].clustno_high, finfo[j].clustno_low);
             get_dm(vfs).fat[m] != end; m = get_dm(vfs).fat[m], k++)
          ;
        void *directory_alloc = malloc(k * get_dm(vfs).ClustnoBytes);
        for (int m = get_clustno(finfo[j].clustno_high, finfo[j].clustno_low), l = 0; l != k; l++) {
          u32 sec1 = (get_dm(vfs).FileDataAddress + (m - 2) * get_dm(vfs).ClustnoBytes) /
                     get_dm(vfs).SectorBytes;
          vdisk_read(sec1, get_dm(vfs).ClustnoBytes / get_dm(vfs).SectorBytes,
                     (char *)directory_alloc + l * get_dm(vfs).SectorBytes, disk_number);
          m = get_dm(vfs).fat[m];
        }
        AddVal((int)directory_alloc, (struct List *)get_dm(vfs).directory_list);
        AddVal(k * get_dm(vfs).ClustnoBytes / 32, (struct List *)get_dm(vfs).directory_max_list);
      }
      if (finfo[j].name[0] == 0) { break; }
    }
  }
  page_free(boot_sector, 512);
}

void Fat_CopyCache(struct vfs_t *dest, struct vfs_t *src) {
  dest->cache = malloc(sizeof(fat_cache));
  memcpy(dest->cache, src->cache, sizeof(fat_cache));
}
bool Fat_cd(struct vfs_t *vfs, char *dictName) {
  return changedict(dictName, vfs);
}
bool Fat_ReadFile(struct vfs_t *vfs, char *path, char *buffer) {
  struct FAT_FILEINFO *finfo;
  finfo = Get_File_Address(path, vfs);
  if (finfo == 0) {
    return 0;
  } else {
    file_loadfile(get_clustno(finfo->clustno_high, finfo->clustno_low), finfo->size, buffer,
                  get_dm(vfs).fat, vfs);
    return 1;
  }
}
bool Fat_WriteFile(struct vfs_t *vfs, char *path, char *buffer, int size) {
  struct FAT_FILEINFO *finfo = Get_File_Address(path, vfs);
  file_savefile(get_clustno(finfo->clustno_high, finfo->clustno_low), size, buffer, get_dm(vfs).fat,
                get_dm(vfs).FatClustnoFlags, vfs);
  finfo->size = size;
  file_saveinfo(Get_dictaddr(path, vfs), vfs);
  return true;
}
List *Fat_ListFile(struct vfs_t *vfs, char *dictpath) {
  struct FAT_FILEINFO *finfo  = Get_dictaddr(dictpath, vfs);
  List                *result = NewList();
  char                 s[30];
  for (int i = 0; i != 30; i++) {
    s[i] = 0;
  }
  for (int i = 0; i < get_directory_max(finfo, vfs); i++) {
    if (finfo[i].name[0] == 0x00) { break; }
    if (finfo[i].name[0] != 0xe5) {
      if ((finfo[i].type & 0x18) == 0 || finfo[i].type == 0x10) {
        for (int j = 0; j < 8; j++) {
          s[j] = finfo[i].name[j];
        }
        s[8] = '.';
        if (finfo[i].ext[0] == ' ') {
          s[8] = 0;
        } else {
          s[9]  = finfo[i].ext[0];
          s[10] = finfo[i].ext[1];
          s[11] = finfo[i].ext[2];
        }
        if (s[0] != '+') {
          vfs_file *d = malloc(sizeof(vfs_file));
          if (finfo[i].type == 0x10) {
            d->type = DIR;
          } else if (finfo[i].type == 0x20) {
            d->type = FLE;
          } else if (finfo[i].type == 0x01) {
            d->type = RDO;
          } else if (finfo[i].type == 0x02) {
            d->type = HID;
          } else if (finfo[i].type == 0x04) {
            d->type = SYS;
          }
          d->year    = (finfo[i].update_date & 65024) >> 9;
          d->year   += 1980;
          d->month   = (finfo[i].update_date & 480) >> 5;
          d->day     = finfo[i].update_date & 31;
          d->hour    = (finfo[i].update_time & 63488) >> 11;
          d->minute  = (finfo[i].update_time & 2016) >> 5;
          d->size    = finfo[i].size;
          int q      = 0;
          for (int k = 0; k < 12 && s[k] != 0; ++k) {
            if (s[k] != ' ') { d->name[q++] = s[k]; }
          }
          d->name[q] = 0;
          // printk("d->name = %s\n", d->name);
          AddVal((u32)d, result);
        }
      }
    }
  }
  return result;
}
bool Fat_RenameFile(struct vfs_t *vfs, char *filename, char *filename_of_new) {
  return rename(filename, filename_of_new, vfs);
}
bool Fat_CreateFile(struct vfs_t *vfs, char *filename) {
  mkfile(filename, vfs);
  return true;
}
void Fat_DeleteFs(struct vfs_t *vfs) {
  free((void *)get_dm(vfs).ADR_DISKIMG);
  free(get_dm(vfs).fat);
  free(get_dm(vfs).FatClustnoFlags);
  free(get_dm(vfs).root_directory);
  DeleteList((List *)get_dm(vfs).directory_clustno_list);
  DeleteList((List *)get_dm(vfs).directory_list);
}
bool Fat_Check(u8 disk_number) {
  //printk("Fat Check\n");
  u8 *boot_sec = malloc(512);
  vdisk_read(0, 1, boot_sec, disk_number);
  if (memcmp(boot_sec + BS_FileSysType, "FAT12   ", 8) == 0 ||
      memcmp(boot_sec + BS_FileSysType, "FAT16   ", 8) == 0 ||
      memcmp(boot_sec + BS_FileSysType + BPB_Fat32ExtByts, "FAT32   ", 8) == 0) {
    free(boot_sec);
    return true;
  }
  free(boot_sec);
  return false;
}
bool Fat_DelFile(struct vfs_t *vfs, char *path) {
  return del(path, vfs);
}
bool Fat_DelDict(struct vfs_t *vfs, char *path) {
  return deldir(path, vfs);
}
int Fat_FileSize(struct vfs_t *vfs, char *filename) {
  if (Get_File_Address(filename, vfs) == NULL) { return -1; }
  return Get_File_Address(filename, vfs)->size;
}
bool Fat_Format(u8 disk_number) {
  return format(disk_number);
}
bool Fat_CreateDict(struct vfs_t *vfs, char *filename) {
  struct FAT_FILEINFO *finfo        = Get_dictaddr(filename, vfs);
  int                  last_clustno = finfo[0].clustno_low;
  if (finfo == get_dm(vfs).root_directory) { last_clustno = 0; }
  mkdir(filename, last_clustno, vfs);
  return true;
}
bool Fat_Attrib(struct vfs_t *vfs, char *filename, ftype type) {
  return attrib(filename, type, vfs);
}
vfs_file *Fat_FileInfo(struct vfs_t *vfs, char *filename) {
  struct FAT_FILEINFO *finfo = Get_File_Address(filename, vfs);
  if (finfo == NULL) { return (vfs_file *)NULL; }
  vfs_file *result = (vfs_file *)malloc(sizeof(vfs_file));
  char      s[30];
  for (int i = 0; i != 30; i++) {
    s[i] = 0;
  }
  for (int i = 0; i < 8; i++) {
    s[i] = finfo->name[i];
  }
  s[8] = '.';
  if (finfo->ext[0] == ' ') {
    s[8] = 0;
  } else {
    s[9]  = finfo->ext[0];
    s[10] = finfo->ext[1];
    s[11] = finfo->ext[2];
  }
  int i = 0;
  for (int j = 0; i < 12 && s[i] != 0; i++) {
    if (s[i] != ' ') { result->name[j++] = s[i]; }
  }
  s[i] = '\0';
  if (finfo->type == 0x10) {
    result->type = DIR;
  } else if (finfo->type == 0x20) {
    result->type = FLE;
  } else if (finfo->type == 0x01) {
    result->type = RDO;
  } else if (finfo->type == 0x02) {
    result->type = HID;
  } else if (finfo->type == 0x04) {
    result->type = SYS;
  }
  result->year    = (finfo->update_date & 65024) >> 9;
  result->year   += 1980;
  result->month   = (finfo->update_date & 480) >> 5;
  result->day     = finfo->update_date & 31;
  result->hour    = (finfo->update_time & 63488) >> 11;
  result->minute  = (finfo->update_time & 2016) >> 5;
  result->size    = finfo->size;
  return result;
}
void Register_fat_fileSys() {
  vfs_t fs;
  fs.flag  = 1;
  fs.cache = NULL;
  strcpy(fs.FSName, "FAT");
  fs.CopyCache  = Fat_CopyCache;
  fs.Format     = Fat_Format;
  fs.CreateFile = Fat_CreateFile;
  fs.CreateDict = Fat_CreateDict;
  fs.DelDict    = Fat_DelDict;
  fs.DelFile    = Fat_DelFile;
  fs.ReadFile   = Fat_ReadFile;
  fs.WriteFile  = Fat_WriteFile;
  fs.DeleteFs   = Fat_DeleteFs;
  fs.cd         = Fat_cd;
  fs.FileSize   = Fat_FileSize;
  fs.Check      = Fat_Check;
  fs.ListFile   = Fat_ListFile;
  fs.InitFs     = fat_InitFS;
  fs.RenameFile = Fat_RenameFile;
  fs.Attrib     = Fat_Attrib;
  fs.FileInfo   = Fat_FileInfo;
  vfs_register_fs(fs);
}