#!/usr/bin/env python3

# 有错误码的中断号
has_errcode = {0x08, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x11, 0x15, 0x1d, 0x1e}

# asm_inthandler_main:
#   ...
#   add esp, 8
#   iret
data = b''
# asm_into_inthandler:
#   push 0                         ; 6a 00
#   jmp short asm_inthandler_main  ; eb xx
# asm_inthandler:
for i in range(0, 256):
  # push (byte)i                   ; 6a xx
  data += b'\x6a' + i.to_bytes(1, byteorder='little', signed=False)
  # jmp near asm_into_inthandler   ; e9 xx xx xx xx 注：如果该中断已有错误码则跳转到 asm_into_inthandler 的后一个语句
  distance = 7 * (i + 1) + (2 if i in has_errcode else 4)
  data += b'\xe9' + (-distance).to_bytes(4, byteorder='little', signed=True)

values = []
for i in range(0, len(data), 4):
  chunk = data[i:i + 4]
  value = int.from_bytes(chunk, byteorder='little')
  values.append(value)

for i in range(0, len(values), 8):
  print('dd 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x' % tuple(values[i:i + 8]))
