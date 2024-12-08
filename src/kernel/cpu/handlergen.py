#!/usr/bin/env python3

data = b''
# into_handler:
#   jmp short handler
for i in range(0, 256):
  # push (byte)i
  push = b'\x6a' + i.to_bytes(1, byteorder='little', signed=False)
  # jmp near into_handler
  jmp = b'\xe9' + (-7 * (i + 1) - 2).to_bytes(4, byteorder='little', signed=True)
  data += push + jmp

values = []
for i in range(0, len(data), 4):
  chunk = data[i:i + 4]
  value = int.from_bytes(chunk, byteorder='little')
  values.append(value)

for i in range(0, len(values), 8):
  print('dd 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x' % tuple(values[i:i + 8]))
