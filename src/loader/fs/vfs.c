#include <dosldr.h>
#define vfs(task)           ((task)->nfs)
#define toupper(c)          ((c) >= 'a' && (c) <= 'z' ? c - 32 : c)
#define PDEBUG(str, ...)    printf(str "\n", ##__VA_ARGS__)
#define WARNING_K(str, ...) printf(str "\n", ##__VA_ARGS__)
#define Panic_K(str, ...)   printf(str "\n", ##__VA_ARGS__)
vfs_t vfsstl[5];
vfs_t vfsMount_Stl[5];

static vfs_t *drive2fs(u8 drive) {
  for (int i = 0; i < 5; i++) {
    if (vfsMount_Stl[i].drive == toupper(drive) && vfsMount_Stl[i].flag == 1) {
      return &vfsMount_Stl[i];
    }
  }
  return NULL;
}

static vfs_t *ParsePath(char *result) {
  PDEBUG("Parse Path: %s", result);
  vfs_t *vfs_result = vfs_now;
  if (result[1] == ':') {
    if (!(vfs_result = drive2fs(result[0]))) {
      WARNING_K("Mount Drive is not found!");
      Panic_K("Parse Error.");
      return NULL;
    }
    if (result) {
      delete_char(result, 0);
      delete_char(result, 0);
    }
  }
  if (result) {
    for (int i = 0; i < strlen(result); i++) {
      if (result[i] == '\\') { result[i] = '/'; }
    }
  }
  PDEBUG("Parse Path OK: %s", result);
  return vfs_result;
}

static vfs_t *findSeat(vfs_t *vstl) {
  for (int i = 0; i < 5; i++) {
    if (vstl[i].flag == 0) { return &vstl[i]; }
  }
  return NULL;
}

static vfs_t *check_disk_fs(u8 disk_number) {
  for (int i = 0; i < 5; i++) {
    if (vfsstl[i].flag == 1) {
      PDEBUG("CALL CHECK %s %08x\n", vfsstl[i].FSName, vfsstl[i].Check);
      if (vfsstl[i].Check(disk_number)) { return &vfsstl[i]; }
    }
  }
  return NULL;
}

static void insert_str(char *str, char *insert_str, int pos) {
  for (int i = 0; i < strlen(insert_str); i++) {
    insert_char(str, pos + i, insert_str[i]);
  }
}

bool vfs_mount_disk(u8 disk_number, u8 drive) {
  PDEBUG("Mount DISK ---- %02x", disk_number);
  for (int i = 0; i < 5; i++) {
    if (vfsMount_Stl[i].flag == 1 &&
        (vfsMount_Stl[i].drive == drive || vfsMount_Stl[i].disk_number == disk_number)) {
      WARNING_K("It mounted");
      return false;
    }
  }
  PDEBUG("OK1");
  vfs_t *seat = findSeat(vfsMount_Stl);
  if (!seat) {
    WARNING_K("can not find a seat of vfsMount_Stl(it's full)");
    Panic_K("Mount error!");
    return false;
  }
  PDEBUG("check\n");
  vfs_t *fs = check_disk_fs(disk_number);
  if (!fs) {
    WARNING_K("unknow file system.");
    Panic_K("Mount error!");
    return false;
  }
  *seat = *fs;
  seat->InitFs(seat, disk_number);
  seat->drive       = drive;
  seat->disk_number = disk_number;
  seat->flag        = 1;
  PDEBUG("success");
  return true;
}

bool vfs_unmount_disk(u8 drive) {
  PDEBUG("Unmount disk ---- %c", drive);
  for (int i = 0; i < 5; i++) {
    if (vfsMount_Stl[i].drive == drive && vfsMount_Stl[i].flag == 1) {
      vfsMount_Stl[i].DeleteFs(&vfsMount_Stl[i]);
      vfsMount_Stl[i].flag = 0;
      PDEBUG("Unmount ok!");
      return true;
    }
  }
  Panic_K("Not found the drive.");
  return false;
}
bool vfs_readfile(char *path, char *buffer) {
  PDEBUG("Readfile %s to %08x\n", path, buffer);
  char *new_path = (char *)page_malloc(strlen(path) + 1);
  strcpy(new_path, path);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(path) + 1);
    return false;
  }
  int result = vfs->ReadFile(vfs, new_path, buffer);
  page_free(new_path, strlen(path) + 1);
  return result;
}
bool vfs_writefile(char *path, char *buffer, int size) {
  char *new_path = (char *)page_malloc(strlen(path) + 1);
  strcpy(new_path, path);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(path) + 1);
    return false;
  }
  int result = vfs->WriteFile(vfs, new_path, buffer, size);
  page_free(new_path, strlen(path) + 1);
  return result;
}
u32 vfs_filesize(char *filename) {
  char *new_path = page_malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(filename) + 1);
    return -1;
  }
  int result = vfs->FileSize(vfs, new_path); // 没找到文件统一返回-1
  page_free(new_path, strlen(filename) + 1);
  return result;
}
List *vfs_listfile(char *dictpath) { // dictpath == "" 则表示当前路径
  if (strcmp(dictpath, "") == 0) {
    return vfs_now->ListFile(vfs_now, dictpath);
  } else {
    char *new_path = page_malloc(strlen(dictpath) + 1);
    strcpy(new_path, dictpath);
    vfs_t *vfs = ParsePath(new_path);
    if (vfs == NULL) {
      WARNING_K("Attempt read a nonexistent disk");
      page_free(new_path, strlen(dictpath) + 1);
      return NULL;
    }
    List *result = vfs->ListFile(vfs, new_path);
    page_free(new_path, strlen(dictpath) + 1);
    return result;
  }
}
bool vfs_delfile(char *filename) {
  PDEBUG("Delete file %s.\n", filename);
  char *new_path = page_malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(filename) + 1);
    return false;
  }
  int result = vfs->DelFile(vfs, new_path);
  page_free(new_path, strlen(filename) + 1);
  return result;
}
bool vfs_deldir(char *dictname) {
  char *new_path = page_malloc(strlen(dictname) + 1);
  strcpy(new_path, dictname);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(dictname) + 1);
    return false;
  }
  int result = vfs->DelDict(vfs, new_path);
  page_free(new_path, strlen(dictname) + 1);
  return result;
}
bool vfs_createfile(char *filename) {
  char *new_path = page_malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(filename) + 1);
    return false;
  }
  int result = vfs->CreateFile(vfs, new_path);
  page_free(new_path, strlen(filename) + 1);
  return result;
}
bool vfs_createdict(char *filename) {
  char *new_path = page_malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(filename) + 1);
    return false;
  }
  int result = vfs->CreateDict(vfs, new_path);
  page_free(new_path, strlen(filename) + 1);
  return result;
}
bool vfs_renamefile(char *filename, char *filename_of_new) {
  char *new_path = page_malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(filename) + 1);
    return false;
  }
  int result = vfs->RenameFile(vfs, new_path, filename_of_new);
  page_free(new_path, strlen(filename) + 1);
  return result;
}
bool vfs_attrib(char *filename, ftype type) {
  char *new_path = page_malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(filename) + 1);
    return false;
  }
  int result = vfs->Attrib(vfs, new_path, type);
  page_free(new_path, strlen(filename) + 1);
  return result;
}
bool vfs_format(u8 disk_number, char *FSName) {
  for (int i = 0; i < 5; i++) {
    if (strcmp(vfsstl[i].FSName, FSName) == 0 && vfsstl[i].flag == 1) {
      return vfsstl[i].Format(disk_number);
    }
  }
  return false;
}
vfs_file *vfs_fileinfo(char *filename) {
  char *new_path = page_malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    WARNING_K("Attempt read a nonexistent disk");
    page_free(new_path, strlen(filename) + 1);
    return NULL;
  }
  vfs_file *result = vfs->FileInfo(vfs, new_path);
  page_free(new_path, strlen(filename) + 1);
  return result;
}
bool vfs_change_disk(u8 drive) {
  PDEBUG("will change to %c", drive);
  if (vfs_now != NULL) {
    while (FindForCount(1, vfs_now->path) != NULL) {
      // printk("%d\n",vfs_now->path->ctl->all);
      page_free((void *)FindForCount(vfs_now->path->ctl->all, vfs_now->path)->val, 255);
      DeleteVal(vfs_now->path->ctl->all, vfs_now->path);
    }
    DeleteList(vfs_now->path);
    page_free(vfs_now, sizeof(vfs_t));
  }
  PDEBUG("Find mount.......");
  vfs_t *f;
  if (!(f = drive2fs(drive))) {
    WARNING_K("no mount.");
    return false; // 没有mount
  }
  PDEBUG("Changing......");
  vfs_now = page_malloc(sizeof(vfs_t));
  memcpy(vfs_now, f, sizeof(vfs_t));
  f->CopyCache(vfs_now, f);
  vfs_now->path = NewList();
  vfs_now->cd(vfs_now, "/");
  PDEBUG("OK.");
  return true;
}
bool vfs_change_disk_for_task(u8 drive, struct TASK *task) {
  PDEBUG("will change to %c", drive);
  if (vfs(task) != NULL) {
    while (FindForCount(1, vfs(task)->path) != NULL) {
      //("%d\n",vfs_now->path->ctl->all);
      page_free((void *)FindForCount(vfs(task)->path->ctl->all, vfs(task)->path)->val, 255);
      DeleteVal(vfs(task)->path->ctl->all, vfs(task)->path);
    }
    DeleteList(vfs(task)->path);
    page_free(vfs(task), sizeof(vfs_t));
  }
  PDEBUG("Find mount.......");
  vfs_t *f;
  if (!(f = drive2fs(drive))) {
    WARNING_K("no mount.");
    return false; // 没有mount
  }
  PDEBUG("Changing......");
  vfs(task) = page_malloc(sizeof(vfs_t));
  memcpy(vfs(task), f, sizeof(vfs_t));
  f->CopyCache(vfs(task), f);
  vfs(task)->path = NewList();
  vfs(task)->cd(vfs(task), "/");
  PDEBUG("OK.");
  return true;
}
bool vfs_change_path(char *dictName) {
  return vfs_now->cd(vfs_now, dictName);
}
void vfs_getPath(char *buffer) {
  char *path;
  List *l;
  buffer[0] = 0;
  insert_char(buffer, 0, vfs_now->drive);
  insert_char(buffer, 1, ':');
  insert_char(buffer, 2, '\\');
  PDEBUG("%s", vfs_now->FSName);
  int pos = strlen(buffer);
  for (int i = 1; FindForCount(i, vfs_now->path) != NULL; i++) {
    l    = FindForCount(i, vfs_now->path);
    path = (char *)l->val;
    insert_str(buffer, path, pos);
    pos += strlen(path);
    insert_char(buffer, pos, '\\');
    pos++;
  }
  delete_char(buffer, pos - 1);
}
bool vfs_check_mount(u8 drive) {
  return drive2fs(drive) ? true : false;
}
void init_vfs() {
  PDEBUG("init vfs..........");
  for (int i = 0; i < 5; i++) {
    vfsstl[i].flag              = 0;
    vfsstl[i].disk_number       = 0;
    vfsstl[i].drive             = 0;
    vfsMount_Stl[i].flag        = 0;
    vfsMount_Stl[i].disk_number = 0;
    vfsMount_Stl[i].drive       = 0;
    // PDEBUG("Set vfsstl[%d] & vfsMount_Stl[%d] OK.", i, i);
  }
  PDEBUG("vfs ok.");
  vfs_now = NULL;
}
bool vfs_register_fs(vfs_t vfs) {
  PDEBUG("Register file system: %s", vfs.FSName);
  PDEBUG("looking for a seat of vfsstl.........");
  vfs_t *seat;
  seat = findSeat(vfsstl);
  if (!seat) {
    WARNING_K("can not find a seat of vfsstl(it's full)");
    Panic_K("Register error!");
    return false;
  }
  *seat = vfs;
  PDEBUG("success");
  return true;
}
