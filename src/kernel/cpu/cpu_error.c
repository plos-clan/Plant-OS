#include <kernel.h>

int  DisableExpFlag = 0;
u32  CatchEIP       = 0;
char flagOfexp      = 0;
char public_catch   = 0;
int  st_task        = 0;

void SwitchPublic() {
  public_catch = 1;
}

void SwitchPrivate() {
  public_catch = 0;
}

void disableExp() {
  if (public_catch) {
    DisableExpFlag = 1;
  } else {
    current_task()->DisableExpFlag = 1;
  }
}

void EnableExp() {
  if (public_catch) {
    DisableExpFlag = 0;
  } else {
    current_task()->DisableExpFlag = 0;
  }
}

char GetExpFlag() {
  // printk("Get.\n");
  if (public_catch) {
    return flagOfexp;
  } else {
    return current_task()->flagOfexp;
  }
}

void ClearExpFlag() {
  if (public_catch) {
    flagOfexp = 0;
  } else {
    current_task()->flagOfexp = 0;
  }
}

void SetCatchEip(u32 eip) {
  // printk("eip = %08x\n",eip);
  if (public_catch) {
    CatchEIP = eip;
  } else {
    current_task()->CatchEIP = eip;
  }
}

void print_32bits_ascil(u32 n);

void insert_char(char *str, int pos, char ch) {
  int i;
  for (i = strlen(str); i >= pos; i--) {
    str[i + 1] = str[i];
  }
  str[pos] = ch;
}
void delete_char(char *str, int pos) {
  int i;
  for (i = pos; i < strlen(str); i++) {
    str[i] = str[i + 1];
  }
}

mtask *last_fpu_task = NULL;
void   fpu_disable() {
  asm_set_ts, asm_set_em;
}

void fpu_enable(mtask *task) {
  asm_clr_ts, asm_clr_em;
  if (!task->fpu_flag) {
    asm volatile("fnclex \n"
                 "fninit \n" ::
                     : "memory");
    memset(&task->fpu, 0, sizeof(fpu_t));
    klogd("FPU create state for task 0x%08x\n", task);
  } else {
    asm volatile("frstor (%%eax) \n" ::"a"(&(task->fpu)) : "memory");
  }
  task->fpu_flag = 1;
}

void ERROR(int CODE, char *TIPS) {
  asm_cli;
  while (true) {}
}

#define _ERROR(_code_, _tips_)                                                                     \
  void ERROR##_code_(u32 eip) {                                                                    \
    u32 *esp = &eip;                                                                               \
    ERROR(_code_, _tips_);                                                                         \
    if (public_catch) {                                                                            \
      *esp = CatchEIP;                                                                             \
    } else {                                                                                       \
      *esp = current_task()->CatchEIP;                                                             \
    }                                                                                              \
  }

_ERROR(0, "#DE");
_ERROR(1, "#DB");
_ERROR(3, "#BP");
_ERROR(4, "#OF");
_ERROR(5, "#BR");
_ERROR(6, "#UD");
_ERROR(8, "#DF");
_ERROR(9, "#MF");
_ERROR(10, "#TS");
_ERROR(11, "#NP");
_ERROR(12, "#SS");
_ERROR(16, "#MF");
_ERROR(17, "#AC");
_ERROR(18, "#MC");
_ERROR(19, "#XF");

#undef _ERROR

int dflag = 0;

bool has_fpu_error() {
  dflag = 0;
  u16 status_word;
  asm("fnstsw %0" : "=m"(status_word));
  dflag = 1;
  return (status_word & 0x1F) != 0;
}

void ERROR7(u32 eip) {
  if (current_task()->fpu_flag > 1 || current_task()->fpu_flag < 0) {
    asm_set_cr0(asm_get_cr0() & ~(CR0_EM | CR0_TS));
    return;
  }
  fpu_enable(current_task());
}

void ERROR13(u32 eip) {
  kloge("ERROR GP!!!!");
  while (true)
    ;
}

void KILLAPP(int eip, int ec) {
  while (true)
    ;
}

void KILLAPP0(int ec, int tn) {
  return;
}
