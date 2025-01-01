#pragma once
#include <define.h>

typedef struct List    List;
typedef struct ListCtl ListCtl;

struct List {
  ListCtl  *ctl;
  List     *prev;
  uintptr_t value;
  List     *next;
};

struct ListCtl {
  List *start;
  List *end;
  int   all;
};

void  AddVal(uintptr_t value, List *Obj);
List *FindForCount(size_t count, List *Obj);
void  DeleteVal(size_t count, List *Obj);
List *NewList();
void  Change(size_t count, List *Obj, uintptr_t value);
int   GetLastCount(List *Obj);
void  DeleteList(List *Obj);