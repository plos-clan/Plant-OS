#!/usr/bin/python3
import os
import json

os.chdir(os.path.dirname(os.path.abspath(__file__)))

next_typeid = 0


class MimeData:

  def __init__(self, mime):
    global next_typeid
    self.id = next_typeid
    self.mime = mime
    self.magic = []
    self.ext = []
    self.des = None
    next_typeid += 1

  def __str__(self):
    color0 = '\033[m'
    color1 = '\033[1;33;40m'
    color2 = '\033[1;32;40m'
    color3 = '\033[1;34;40m'
    color4 = '\033[1;35;40m'
    return f"{color1}{self.mime}{color0};{color2}{self.magic}{color0};{color3}{self.ext}{color0};{color4}{self.des}{color0}"


mimes: dict[str, MimeData] = {}

lines = []

with open('magic.csv', 'r') as f:
  for line in f:
    if len(line.strip()) == 0: continue
    lines.append(line.strip())

with open('magic.plos.csv', 'r') as f:
  for line in f:
    if len(line.strip()) == 0: continue
    lines.append(line.strip())

for line in lines:
  if len(line.strip()) == 0: continue
  line = line.strip().split(';')
  if line[0] == 'MIME': continue
  mine = line[0]
  type = line[1]
  magic = line[2]
  ext = line[3]
  des = line[4]
  if mine not in mimes: mimes[mine] = MimeData(mine)
  data = mimes[mine]
  if not type and magic: raise "error"
  if type and not magic: raise "error"
  if type == 'str': data.magic.append(magic.encode())
  if type == 'hex': data.magic.append(bytes.fromhex(magic))
  if ext: data.ext += ext.split(',')
  if des: data.des = des

mimes_list: list[MimeData] = [None] * next_typeid

for data in mimes.values():
  mimes_list[data.id] = data

code = '''
#include <define.h>

static const cstr mime_types[] = {
$$ mime_types $$
};

static const cstr descriptions[] = {
$$ descriptions $$
};

dlexport cstr filetype(const void *data, size_t size) $$ fn-filetype $$
'''


def gencode(prefix: bytes, nchar: int):
  d, e = {}, None
  for data in mimes_list:
    for magic in data.magic:
      if magic.startswith(prefix):
        if len(magic) != len(prefix):
          d[magic[nchar]] = None
        else:
          e = f'return mime_types[{data.id}];'
  if len(d) == 0: return e
  code = []
  if not e: code.append(f'if (size == {nchar}) return null;')
  code.append('byte b = ((const byte *)data)[' + str(nchar) + '];')
  for i in d:
    code.append(f'if (b == {i}) {gencode(prefix + bytes([i]), nchar + 1)}')
  if e: code.append(e)
  if not e: code.append('return null;')
  code = ['  ' * (nchar + 1) + s for s in code]
  code = '\n'.join(['{'] + code + ['  ' * nchar + '}'])
  return code


code = code.replace('$$ mime_types $$', '\n'.join([f'    "{data.mime}",' for data in mimes_list]))
code = code.replace('$$ descriptions $$', '\n'.join([f'    "{data.des}",' for data in mimes_list]))
code = code.replace('$$ fn-filetype $$', gencode(b'', 0))

with open('filetype.c', 'w') as f:
  f.write(code)
