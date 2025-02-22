#include <elf.h>
#include <kernel.h>

#define PORT 0x3f8

__nif static void _putb(int c) {
  waituntil(asm_in8(PORT + 5) & 0x20);
  asm_out8(PORT, c);
}

__nif static void _puts(cstr s) {
  for (size_t i = 0; s[i] != '\0'; i++) {
    _putb(s[i]);
  }
}

__nif static void _print(cstr _rest fmt, ...) {
  static char buf[128];
  va_list     va;
  va_start(va, fmt);
  vsprintf(buf, fmt, va);
  va_end(va);
  _puts(buf);
}

static volatile bool paniced = false;

__nif void __cyg_profile_func_enter(void *callee, void *caller) {
  if (!current_task || paniced) return;
  current_task->call_log[current_task->call_log_ptr].addr   = (usize)callee;
  current_task->call_log[current_task->call_log_ptr].is_ret = false;
  current_task->call_log_ptr++;
  if (current_task->call_log_ptr == CALL_LOG_MAX) current_task->call_log_ptr = 0;
}

__nif void __cyg_profile_func_exit(void *callee, void *caller) {
  if (!current_task || paniced) return;
  current_task->call_log[current_task->call_log_ptr].addr   = (usize)callee;
  current_task->call_log[current_task->call_log_ptr].is_ret = true;
  current_task->call_log_ptr++;
  if (current_task->call_log_ptr == CALL_LOG_MAX) current_task->call_log_ptr = 0;
}

static const char        *strtab;
static const Elf32Symbol *symtab;
static usize              strtab_size;
static usize              symtab_size;

__nif static void readelf(const Elf32Header *header) {
  val sections = (const Elf32SectionHeader *)((usize)header + header->shoff);
  for (usize i = 0; i < header->shnum; i++) {
    val section = sections + i;
    if (section->type == ELF_SECTION_TYPE_STRTAB) {
      strtab      = (const char *)((usize)header + section->offset);
      strtab_size = section->size;
    }
    if (section->type == ELF_SECTION_TYPE_SYMTAB) {
      symtab      = (const Elf32Symbol *)((usize)header + section->offset);
      symtab_size = section->size;
    }
  }
}

__nif static cstr getfuncname(usize addr) {
  for (usize i = 0; i < symtab_size / sizeof(Elf32Symbol); i++) {
    if (symtab[i].value == addr) return strtab + symtab[i].name;
  }
  return "<unknown>";
}

#if PLOS_LOGGING == 2
#endif

void *kernel_data = null;

void panic() {
  asm_cli;
  paniced = true;

  klogf("panic");
  klogi("current_task %d", current_task->tid);

  int max_depth = 0, min_depth = 0, depth = 0;
  for (usize i = current_task->call_log_ptr + 1; i != current_task->call_log_ptr; i++) {
    if (i == CALL_LOG_MAX) i = 0;
    if (current_task->call_log[i].is_ret) {
      depth--;
      if (depth < min_depth) min_depth = depth;
    } else {
      depth++;
      if (depth > max_depth) max_depth = depth;
    }
  }

  readelf(kernel_data);

  static cstr colors[] = {
      "\033[38;2;255;255;64m",
      "\033[38;2;127;255;127m",
      "\033[38;2;255;127;255m",
      "\033[38;2;79;236;236m",
  };

  _putb('\n');
  _puts("call process:\n");

  depth = 0;
  for (isize i = current_task->call_log_ptr + 1; i != current_task->call_log_ptr; i++) {
    if (i == CALL_LOG_MAX) i = 0;
    if (current_task->call_log[i].is_ret) {
      depth--;
      for (int j = min_depth; j < depth; j++) {
        _puts(colors[(j - min_depth) % 4]);
        _puts("|\033[0m ");
      }
      _puts("<- ");
      _puts(colors[(depth - min_depth) % 4]);
      _puts(getfuncname(current_task->call_log[i].addr));
      _puts("\033[0m\n");
    } else {
      for (int j = min_depth; j < depth; j++) {
        _puts(colors[(j - min_depth) % 4]);
        _puts("|\033[0m ");
      }
      _puts("-> ");
      _puts(colors[(depth - min_depth) % 4]);
      _puts(getfuncname(current_task->call_log[i].addr));
      _puts("\033[0m\n");
      depth++;
    }
  }

  infinite_loop asm_hlt;
}
