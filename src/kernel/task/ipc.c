#include <kernel.h>

void ipc_send(i32 pid, i32 msgid, const void *data, size_t size, bool switch_task) {
  TODO();
}

i32 ipc_recv(i32 msgid, void *data, size_t size, bool wait) {
  TODO();
}
