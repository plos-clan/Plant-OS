#include <kernel.h>

#define vfs(task) ((task)->nfs)

vfs_t vfsstl[26];
vfs_t vfsMount_Stl[26];

static vfs_t *drive2fs(u8 drive) {
  for (int i = 0; i < 26; i++) {
    if (vfsMount_Stl[i].drive == toupper(drive) && vfsMount_Stl[i].flag == 1) {
      return &vfsMount_Stl[i];
    }
  }
  return NULL;
}

static void insert_char(char *str, size_t pos, char ch) {
  for (size_t i = strlen(str); i >= pos; i--) {
    str[i + 1] = str[i];
  }
  str[pos] = ch;
}

static void delete_char(char *str, size_t pos) {
  size_t len = strlen(str);
  for (size_t i = pos; i < len; i++) {
    str[i] = str[i + 1];
  }
}

static vfs_t *ParsePath(char *result) {
  logd("Parse Path: %s", result);
  vfs_t *vfs_result = vfs_now;
  if (result[1] == ':') {
    if (!(vfs_result = drive2fs(result[0]))) {
      logw("Mount Drive is not found!");
      loge("Parse Error.");
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
  logd("Parse Path OK: %s", result);
  return vfs_result;
}

static vfs_t *findSeat(vfs_t *vstl) {
  for (int i = 0; i < 26; i++) {
    if (vstl[i].flag == 0) { return &vstl[i]; }
  }
  return NULL;
}

static vfs_t *check_disk_fs(u8 disk_number) {
  for (int i = 0; i < 26; i++) {
    if (vfsstl[i].flag == 1) {
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
  logd("Mount DISK ---- %02x", disk_number);
  for (int i = 0; i < 26; i++) {
    if (vfsMount_Stl[i].flag == 1 &&
        (vfsMount_Stl[i].drive == drive || vfsMount_Stl[i].disk_number == disk_number)) {
      logw("It has been mounted");
      return false;
    }
  }
  vfs_t *seat = findSeat(vfsMount_Stl);
  if (!seat) {
    logw("can not find a seat of vfsMount_Stl(it's full)");
    loge("Mount error!");
    return false;
  }
  vfs_t *fs = check_disk_fs(disk_number);
  if (!fs) {
    logw("unknow file system.");
    loge("Mount error!");
    return false;
  }
  *seat = *fs;
  seat->InitFs(seat, disk_number);
  seat->drive       = drive;
  seat->disk_number = disk_number;
  seat->flag        = 1;
  logd("success");
  return true;
}

bool vfs_unmount_disk(u8 drive) {
  logd("Unmount disk ---- %c", drive);
  for (int i = 0; i < 26; i++) {
    if (vfsMount_Stl[i].drive == drive && vfsMount_Stl[i].flag == 1) {
      vfsMount_Stl[i].DeleteFs(&vfsMount_Stl[i]);
      vfsMount_Stl[i].flag = 0;
      logd("Unmount ok!");
      return true;
    }
  }
  loge("Not found the drive.");
  return false;
}

bool vfs_readfile(char *path, char *buffer) {
  logd("Readfile %s to %08x\n", path, buffer);
  char *new_path = malloc(strlen(path) + 1);
  strcpy(new_path, path);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return false;
  }
  int result = vfs->ReadFile(vfs, new_path, buffer);
  free(new_path);
  logd("OK\n");
  return result;
}

bool vfs_writefile(char *path, char *buffer, int size) {
  char *new_path = malloc(strlen(path) + 1);
  strcpy(new_path, path);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return false;
  }
  int result = vfs->WriteFile(vfs, new_path, buffer, size);
  free(new_path);
  return result;
}

u32 vfs_filesize(char *filename) {
  char *new_path = malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return -1;
  }
  int result = vfs->FileSize(vfs, new_path); // 没找到文件统一返回-1
  free(new_path);
  return result;
}

list_t vfs_listfile(char *dictpath) { // dictpath == "" 则表示当前路径
  if (strcmp(dictpath, "") == 0) {
    return vfs_now->ListFile(vfs_now, dictpath);
  } else {
    char *new_path = malloc(strlen(dictpath) + 1);
    strcpy(new_path, dictpath);
    vfs_t *vfs = ParsePath(new_path);
    if (vfs == NULL) {
      logw("Attempt read a nonexistent disk");
      free(new_path);
      return NULL;
    }
    list_t result = vfs->ListFile(vfs, new_path);
    free(new_path);
    return result;
  }
}

bool vfs_delfile(char *filename) {
  logd("Delete file %s.\n", filename);
  char *new_path = malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return false;
  }
  int result = vfs->DelFile(vfs, new_path);
  free(new_path);
  return result;
}

bool vfs_deldir(char *dictname) {
  char *new_path = malloc(strlen(dictname) + 1);
  strcpy(new_path, dictname);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return false;
  }
  int result = vfs->DelDict(vfs, new_path);
  free(new_path);
  return result;
}

bool vfs_createfile(char *filename) {
  char *new_path = malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return false;
  }
  int result = vfs->CreateFile(vfs, new_path);
  free(new_path);
  return result;
}

bool vfs_createdict(char *filename) {
  char *new_path = malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return false;
  }
  int result = vfs->CreateDict(vfs, new_path);
  free(new_path);
  return result;
}

bool vfs_renamefile(char *filename, char *filename_of_new) {
  char *new_path = malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return false;
  }
  int result = vfs->RenameFile(vfs, new_path, filename_of_new);
  free(new_path);
  return result;
}

bool vfs_attrib(char *filename, ftype type) {
  char *new_path = malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return false;
  }
  int result = vfs->Attrib(vfs, new_path, type);
  free(new_path);
  return result;
}

bool vfs_format(u8 disk_number, char *FSName) {
  for (int i = 0; i < 255; i++) {
    if (strcmp(vfsstl[i].FSName, FSName) == 0 && vfsstl[i].flag == 1) {
      return vfsstl[i].Format(disk_number);
    }
  }
  return false;
}

vfs_file *vfs_fileinfo(char *filename) {
  char *new_path = malloc(strlen(filename) + 1);
  strcpy(new_path, filename);
  vfs_t *vfs = ParsePath(new_path);
  if (vfs == NULL) {
    logw("Attempt read a nonexistent disk");
    free(new_path);
    return NULL;
  }
  vfs_file *result = vfs->FileInfo(vfs, new_path);
  free(new_path);
  return result;
}

bool vfs_change_disk(u8 drive) {
  logd("will change to %c", drive);
  if (vfs_now != NULL) {
    list_free_with(vfs_now->path, free);
    free(vfs_now->cache);
    free(vfs_now);
  }
  logd("Find mount.......");
  vfs_t *f;
  if (!(f = drive2fs(drive))) {
    logw("no mount.");
    return false; // 没有mount
  }
  logd("Changing......");
  vfs_now = malloc(sizeof(vfs_t));
  memcpy(vfs_now, f, sizeof(vfs_t));
  f->CopyCache(vfs_now, f);
  vfs_now->path = NULL;
  vfs_now->cd(vfs_now, "/");
  logd("OK.");
  return true;
}

bool vfs_change_disk_for_task(u8 drive, mtask *task) {
  logd("will change to %c", drive);
  if (vfs(task) != NULL) {
    list_free_with(vfs(task)->path, free);
    free(vfs(task)->cache);
    free(vfs(task));
  }
  logd("Find mount.......");
  vfs_t *f;
  if (!(f = drive2fs(drive))) {
    logw("no mount.");
    return false; // 没有mount
  }
  logd("Changing......");
  vfs(task) = malloc(sizeof(vfs_t));
  memcpy(vfs(task), f, sizeof(vfs_t));
  f->CopyCache(vfs(task), f);
  vfs(task)->path = NULL;
  vfs(task)->cd(vfs(task), "/");
  logd("OK.");
  return true;
}

bool vfs_change_path(char *dictName) {
  char *buf = malloc(strlen(dictName) + 1);
  char *r   = buf;
  memcpy(buf, dictName, strlen(dictName) + 1);
  int i = 0;
  if (buf[i] == '/' || buf[i] == '\\') {
    if (!vfs_now->cd(vfs_now, "/")) {
      free(r);
      return false;
    }
    i++;
    buf++;
  }

  for (;; i++) {
    if (buf[i] == '/' || buf[i] == '\\') {
      buf[i] = 0;
      if (!vfs_now->cd(vfs_now, buf)) {
        free(r);
        return false;
      }
      buf += strlen(buf) + 1;
    }
    if (buf[i] == 0) {
      if (!vfs_now->cd(vfs_now, buf)) {
        free(r);
        return false;
      }
      break;
    }
  }
  free(r);
  return true;
}

void vfs_getPath(char *buffer) {
  char  *path;
  list_t l;
  buffer[0] = 0;
  insert_char(buffer, 0, vfs_now->drive);
  insert_char(buffer, 1, ':');
  insert_char(buffer, 2, '\\');
  logd("%s", vfs_now->FSName);
  int pos = strlen(buffer);
  list_foreach(vfs_now->path, path) {
    insert_str(buffer, path->data, pos);
    pos += strlen(path->data);
    insert_char(buffer, pos, '\\');
    pos++;
  }
  delete_char(buffer, pos - 1);
}

void vfs_getPath_no_drive(char *buffer) {
  char  *path;
  list_t l;
  buffer[0] = 0;
  logd("%s", vfs_now->FSName);
  int pos = strlen(buffer);
  int i;
  list_foreach(vfs_now->path, path) {
    insert_char(buffer, pos, '/');
    pos++;
    insert_str(buffer, path->data, pos);
    pos += strlen(path->data);
  }
  if (i == 1) { insert_char(buffer, 0, '/'); }
}

bool vfs_check_mount(u8 drive) {
  return drive2fs(drive) ? true : false;
}

void init_vfs() {
  printi("init vfs..........");
  for (int i = 0; i < 26; i++) {
    vfsstl[i].flag              = 0;
    vfsstl[i].disk_number       = 0;
    vfsstl[i].drive             = 0;
    vfsMount_Stl[i].flag        = 0;
    vfsMount_Stl[i].disk_number = 0;
    vfsMount_Stl[i].drive       = 0;
    // logd("Set vfsstl[%d] & vfsMount_Stl[%d] OK.", i, i);
  }
  printi("vfs ok.");
  vfs_now = NULL;
}

bool vfs_register_fs(vfs_t vfs) {
  logd("Register file system: %s", vfs.FSName);
  logd("looking for a seat of vfsstl.........");
  vfs_t *seat;
  seat = findSeat(vfsstl);
  if (!seat) {
    logw("can not find a seat of vfsstl(it's full)");
    loge("Register error!");
    return false;
  }
  *seat = vfs;
  printi("succeed in registering a fs");
  return true;
}
